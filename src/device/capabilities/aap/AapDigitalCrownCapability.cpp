// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapDigitalCrownCapability.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/setters/AapSetDigitalCrown.h"

namespace MagicPodsCore
{
    static constexpr auto DigitalCrownSetting = "digital_crown";

    std::string AapDigitalCrownCapability::OptionToString(int value)
    {
        if (value == DoNotApply)
            return "DoNotApply";
        if (value >= 0 && value <= 0xff &&
            IsValidAapDigitalCrownMode(static_cast<unsigned char>(value)))
        {
            return AapDigitalCrownModeToString(static_cast<AapDigitalCrownMode>(value));
        }
        return "Unknown value";
    }

    nlohmann::json AapDigitalCrownCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = _option;
        return bodyJson;
    }

    void AapDigitalCrownCapability::OnReceivedData(const std::vector<unsigned char>&)
    {
    }

    AapDigitalCrownCapability::AapDigitalCrownCapability(AapDevice& device)
        : AapCapability("digitalCrown", false, device)
    {
        _clientStateChangedEventId = device.GetClientStateChangedEvent().Subscribe(
            [this](size_t, ClientState state)
            {
                if (state != ClientState::Connected)
                    return;

                _option = DoNotApply;
                const auto setting = this->device.LoadSettingInt(DigitalCrownSetting);
                if (setting.has_value())
                {
                    const auto value = setting.value();
                    if (value == DoNotApply ||
                        (value >= 0 && value <= 0xff &&
                         IsValidAapDigitalCrownMode(static_cast<unsigned char>(value))))
                    {
                        _option = static_cast<int>(value);
                    }
                    else
                    {
                        Logger::Error("DigitalCrown setting has unexpected value: %lld",
                            static_cast<long long>(value));
                    }
                }

                if (_option != DoNotApply)
                    SendData(AapSetDigitalCrown(static_cast<AapDigitalCrownMode>(_option)));

                isAvailable = true;
                Logger::Info("DigitalCrown updated: %s", OptionToString(_option).c_str());
                _onChanged.FireEvent(*this);
            });
    }

    AapDigitalCrownCapability::~AapDigitalCrownCapability()
    {
        device.GetClientStateChangedEvent().Unsubscribe(_clientStateChangedEventId);
    }

    bool AapDigitalCrownCapability::IsSupported(unsigned short model)
    {
        switch (static_cast<AapModelIds>(model))
        {
        case AapModelIds::airpodsmax:
        case AapModelIds::airpodsmax2024:
        case AapModelIds::airpodsmax2:
            return true;
        default:
            return false;
        }
    }

    void AapDigitalCrownCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_number_integer())
        {
            Logger::Error("Error: AapDigitalCrownCapability::SetFromJson got no value or value is not an integer");
            return;
        }

        const int selected = capability["selected"].get<int>();
        if (selected != DoNotApply &&
            (selected < 0 || selected > 0xff ||
             !IsValidAapDigitalCrownMode(static_cast<unsigned char>(selected))))
        {
            Logger::Error("Error: AapDigitalCrownCapability::SetFromJson got unexpected option: %d", selected);
            return;
        }

        if (selected != DoNotApply)
            SendData(AapSetDigitalCrown(static_cast<AapDigitalCrownMode>(selected)));
        device.SaveSettingInt(DigitalCrownSetting, selected);

        if (_option != selected)
        {
            _option = selected;
            Logger::Info("DigitalCrown updated: %s", OptionToString(_option).c_str());
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("AapDigitalCrownCapability::SetFromJson set option to %s",
            OptionToString(_option).c_str());
    }
}
