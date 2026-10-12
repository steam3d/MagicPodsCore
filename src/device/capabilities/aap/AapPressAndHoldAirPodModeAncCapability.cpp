// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPressAndHoldAirPodModeAncCapability.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/enums/AapPressAndHoldAirPodModeAnc.h"
#include "sdk/aap/setters/AapSetAnc.h"
#include "sdk/aap/setters/AapSetPressAndHoldAirPodModeAnc.h"

namespace MagicPodsCore
{
    static constexpr auto PressAndHoldAirPodModeAncSetting = "press_and_hold_airpod_mode_anc";

    unsigned char AapPressAndHoldAirPodModeAncCapability::ModeToFlag(AapAncMode mode)
    {
        switch (mode)
        {
        case AapAncMode::Off:
            return static_cast<unsigned char>(AapPressAndHoldAirPodModeAnc::Off);
        case AapAncMode::Anc:
            return static_cast<unsigned char>(AapPressAndHoldAirPodModeAnc::Anc);
        case AapAncMode::Transparency:
            return static_cast<unsigned char>(AapPressAndHoldAirPodModeAnc::Transparency);
        case AapAncMode::Adaptive:
            return static_cast<unsigned char>(AapPressAndHoldAirPodModeAnc::Adaptive);
        default:
            return 0;
        }
    }

    unsigned char AapPressAndHoldAirPodModeAncCapability::GetAvailableModes() const
    {
        unsigned char options = 0;
        for (const auto mode : AapSetAnc::GetAncModesFor(device.GetProductId()))
            options |= ModeToFlag(mode);
        return options;
    }

    bool AapPressAndHoldAirPodModeAncCapability::HasAtLeastTwoModes(int value)
    {
        const auto modes = static_cast<unsigned char>(value);
        return modes != 0 && (modes & static_cast<unsigned char>(modes - 1)) != 0;
    }

    bool AapPressAndHoldAirPodModeAncCapability::IsValidOption(int value) const
    {
        if (value == DoNotApply)
            return true;
        if (value < 0 || value > 0xff)
            return false;

        const auto modes = static_cast<unsigned char>(value);
        return (modes & static_cast<unsigned char>(~GetAvailableModes())) == 0;
    }

    nlohmann::json AapPressAndHoldAirPodModeAncCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = _option;
        bodyJson["options"] = GetAvailableModes();
        return bodyJson;
    }

    void AapPressAndHoldAirPodModeAncCapability::OnReceivedData(const std::vector<unsigned char>&)
    {
    }

    AapPressAndHoldAirPodModeAncCapability::AapPressAndHoldAirPodModeAncCapability(AapDevice& device)
        : AapCapability("pressAndHoldAirPodModeAnc", false, device)
    {
        _clientStateChangedEventId = device.GetClientStateChangedEvent().Subscribe(
            [this](size_t, ClientState state)
            {
                if (state != ClientState::Connected)
                    return;

                _option = DoNotApply;
                const auto setting = this->device.LoadSettingInt(PressAndHoldAirPodModeAncSetting);
                if (setting.has_value())
                {
                    const auto value = setting.value();
                    if (value >= DoNotApply && value <= 0xff && IsValidOption(static_cast<int>(value)))
                    {
                        const int savedOption = static_cast<int>(value);
                        _option = savedOption == DoNotApply || HasAtLeastTwoModes(savedOption)
                            ? savedOption
                            : DoNotApply;
                    }
                    else
                        Logger::Error("PressAndHoldAirPodModeAnc setting has unexpected value: %lld",
                            static_cast<long long>(value));
                }

                if (_option != DoNotApply)
                    SendData(AapSetPressAndHoldAirPodModeAnc(static_cast<unsigned char>(_option)));

                isAvailable = true;
                Logger::Info("PressAndHoldAirPodModeAnc updated: %d", _option);
                _onChanged.FireEvent(*this);
            });
    }

    AapPressAndHoldAirPodModeAncCapability::~AapPressAndHoldAirPodModeAncCapability()
    {
        device.GetClientStateChangedEvent().Unsubscribe(_clientStateChangedEventId);
    }

    bool AapPressAndHoldAirPodModeAncCapability::IsSupported(unsigned short model)
    {
        switch (static_cast<AapModelIds>(model))
        {
        case AapModelIds::airpodspro:
        case AapModelIds::airpodspro2:
        case AapModelIds::airpodspro3:
        case AapModelIds::airpodsprousbc:
        case AapModelIds::airpodsmax:
        case AapModelIds::beatsfitpro:
        case AapModelIds::airpodsmax2024:
        case AapModelIds::airpods4anc:
        case AapModelIds::airpods5wcc:
        case AapModelIds::airpods5:
        case AapModelIds::powerbeatspro2:
        case AapModelIds::powerbeatsfit:
        case AapModelIds::airpodsmax2:
            return true;
        default:
            return false;
        }
    }

    void AapPressAndHoldAirPodModeAncCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_number_integer())
        {
            Logger::Error("Error: AapPressAndHoldAirPodModeAncCapability::SetFromJson got no value or value is not an integer");
            return;
        }

        int selected = capability["selected"].get<int>();
        if (!IsValidOption(selected))
        {
            Logger::Error("Error: AapPressAndHoldAirPodModeAncCapability::SetFromJson got unexpected option: %d",
                selected);
            return;
        }

        if (selected != DoNotApply && !HasAtLeastTwoModes(selected))
            selected = DoNotApply;

        if (selected != DoNotApply)
            SendData(AapSetPressAndHoldAirPodModeAnc(static_cast<unsigned char>(selected)));
        device.SaveSettingInt(PressAndHoldAirPodModeAncSetting, selected);

        if (_option != selected)
        {
            _option = selected;
            Logger::Info("PressAndHoldAirPodModeAnc updated: %d", _option);
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("AapPressAndHoldAirPodModeAncCapability::SetFromJson set option to %d", _option);
    }
}
