// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapAncOffModeCapability.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/setters/AapSetAncOffMode.h"

namespace MagicPodsCore
{
    static constexpr auto AncOffModeSetting = "anc_off_mode";

    std::string AapAncOffModeCapability::OptionToString(int value)
    {
        if (value == DoNotApply)
            return "DoNotApply";
        if (value >= 0 && value <= 0xff && IsValidAapAncOffMode(static_cast<unsigned char>(value)))
            return AapAncOffModeToString(static_cast<AapAncOffMode>(value));
        return "Unknown value";
    }

    nlohmann::json AapAncOffModeCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapAncOffModeCapability::OnReceivedData(const std::vector<unsigned char>&)
    {
    }

    AapAncOffModeCapability::AapAncOffModeCapability(AapDevice& device) : AapCapability("ancOffMode", false, device)
    {
        clientStateChangedEventId = device.GetClientStateChangedEvent().Subscribe([this](size_t, ClientState state){
            if (state == ClientState::Connected){
                option = DoNotApply; //default value
                const auto setting = this->device.LoadSettingInt(AncOffModeSetting);
                if (setting.has_value())
                {
                    const auto value = setting.value();
                    if (value == DoNotApply ||
                        (value >= 0 && value <= 0xff && IsValidAapAncOffMode(static_cast<unsigned char>(value))))
                    {
                        option = static_cast<int>(value);
                    }
                    else
                    {
                        Logger::Error("AncOffMode setting has unexpected value: %lld", static_cast<long long>(value));
                    }
                }

                // The current value cannot be read from the headphones. Apply only an explicit preference.
                if (option != DoNotApply)
                    SendData(AapSetAncOffMode(static_cast<AapAncOffMode>(option)));
                isAvailable = true;
                Logger::Info("AncOffMode updated: %s", OptionToString(option).c_str());
                _onChanged.FireEvent(*this);
            }
        });
    }

    AapAncOffModeCapability::~AapAncOffModeCapability()
    {
        device.GetClientStateChangedEvent().Unsubscribe(clientStateChangedEventId);
    }

    bool AapAncOffModeCapability::IsSupported(unsigned short model)
    {
        switch (static_cast<AapModelIds>(model))
        {
        case AapModelIds::airpodspro2:
        case AapModelIds::airpodspro3:
        case AapModelIds::airpodsprousbc:
            return true;
        default:
            return false;
        }
    }

    void AapAncOffModeCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_number_integer())
        {
            Logger::Error("Error: AapAncOffModeCapability::SetFromJson got no value or value is not an integer");
            return;
        }

        const int selected = capability["selected"].get<int>();
        if (selected != DoNotApply &&
            (selected < 0 || selected > 0xff || !IsValidAapAncOffMode(static_cast<unsigned char>(selected))))
        {
            Logger::Error("Error: AapAncOffModeCapability::SetFromJson got unexpected option: %d", selected);
            return;
        }

        if (selected != DoNotApply)
            SendData(AapSetAncOffMode(static_cast<AapAncOffMode>(selected)));
        device.SaveSettingInt(AncOffModeSetting, selected);

        if (option != selected){
            option = selected;
            Logger::Info("AncOffMode updated: %s", OptionToString(option).c_str());
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("AapAncOffModeCapability::SetFromJson set option to %s", OptionToString(option).c_str());
    }
}
