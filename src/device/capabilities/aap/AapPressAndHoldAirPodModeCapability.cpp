// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPressAndHoldAirPodModeCapability.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/setters/AapSetPressAndHoldAirPodMode.h"

namespace MagicPodsCore
{
    static constexpr auto RightSetting = "press_and_hold_airpod_mode_right";
    static constexpr auto LeftSetting = "press_and_hold_airpod_mode_left";

    bool AapPressAndHoldAirPodModeCapability::IsValidOption(int value)
    {
        return value == DoNotApply ||
               (value >= 0 && value <= 0xff &&
                IsValidAapPressAndHoldAirPodMode(static_cast<unsigned char>(value)));
    }

    std::string AapPressAndHoldAirPodModeCapability::OptionToString(int value)
    {
        if (value == DoNotApply)
            return "DoNotApply";
        if (value >= 0 && value <= 0xff &&
            IsValidAapPressAndHoldAirPodMode(static_cast<unsigned char>(value)))
        {
            return AapPressAndHoldAirPodModeToString(
                static_cast<AapPressAndHoldAirPodMode>(value));
        }
        return "Unknown value";
    }

    void AapPressAndHoldAirPodModeCapability::SendPreference()
    {
        // Both bytes must be sent together. Do not overwrite either bud when its
        // current value is unknown and no preference has been configured for it.
        if (_rightOption == DoNotApply || _leftOption == DoNotApply)
            return;

        SendData(AapSetPressAndHoldAirPodMode(
            static_cast<AapPressAndHoldAirPodMode>(_rightOption),
            static_cast<AapPressAndHoldAirPodMode>(_leftOption)));
    }

    nlohmann::json AapPressAndHoldAirPodModeCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = {
            {"right", _rightOption},
            {"left", _leftOption}
        };
        return bodyJson;
    }

    void AapPressAndHoldAirPodModeCapability::OnReceivedData(const std::vector<unsigned char>&)
    {
    }

    AapPressAndHoldAirPodModeCapability::AapPressAndHoldAirPodModeCapability(AapDevice& device)
        : AapCapability("pressAndHoldAirPodMode", false, device)
    {
        _clientStateChangedEventId = device.GetClientStateChangedEvent().Subscribe(
            [this](size_t, ClientState state)
            {
                if (state != ClientState::Connected)
                    return;

                _rightOption = DoNotApply; //default value
                _leftOption = DoNotApply; //default value

                const auto rightSetting = this->device.LoadSettingInt(RightSetting);
                if (rightSetting.has_value())
                {
                    const auto value = rightSetting.value();
                    if (value >= DoNotApply && value <= 0xff && IsValidOption(static_cast<int>(value)))
                        _rightOption = static_cast<int>(value);
                    else
                        Logger::Error("PressAndHoldAirPodMode right setting has unexpected value: %lld",
                            static_cast<long long>(value));
                }

                const auto leftSetting = this->device.LoadSettingInt(LeftSetting);
                if (leftSetting.has_value())
                {
                    const auto value = leftSetting.value();
                    if (value >= DoNotApply && value <= 0xff && IsValidOption(static_cast<int>(value)))
                        _leftOption = static_cast<int>(value);
                    else
                        Logger::Error("PressAndHoldAirPodMode left setting has unexpected value: %lld",
                            static_cast<long long>(value));
                }

                // The protocol always writes both buds in one command. A partial
                // DoNotApply state cannot be honored without overwriting one bud.
                if (_rightOption == DoNotApply || _leftOption == DoNotApply)
                {
                    _rightOption = DoNotApply;
                    _leftOption = DoNotApply;
                }

                SendPreference();
                isAvailable = true;
                Logger::Info("PressAndHoldAirPodMode updated: right=%s, left=%s",
                    OptionToString(_rightOption).c_str(), OptionToString(_leftOption).c_str());
                _onChanged.FireEvent(*this);
            });
    }

    AapPressAndHoldAirPodModeCapability::~AapPressAndHoldAirPodModeCapability()
    {
        device.GetClientStateChangedEvent().Unsubscribe(_clientStateChangedEventId);
    }

    bool AapPressAndHoldAirPodModeCapability::IsSupported(unsigned short model)
    {
        switch (static_cast<AapModelIds>(model))
        {
        case AapModelIds::airpodspro:
        case AapModelIds::airpodspro2:
        case AapModelIds::airpodspro3:
        case AapModelIds::airpodsprousbc:
        case AapModelIds::beatsfitpro:
        case AapModelIds::airpods4anc:
        case AapModelIds::airpods5wcc:
        case AapModelIds::airpods5:
        case AapModelIds::powerbeatspro2:
        case AapModelIds::powerbeatsfit:
            return true;
        default:
            return false;
        }
    }

    void AapPressAndHoldAirPodModeCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_object())
        {
            Logger::Error("Error: AapPressAndHoldAirPodModeCapability::SetFromJson got no selected object");
            return;
        }

        const auto& selected = capability["selected"];
        if (!selected.contains("right") || !selected["right"].is_number_integer() ||
            !selected.contains("left") || !selected["left"].is_number_integer())
        {
            Logger::Error("Error: AapPressAndHoldAirPodModeCapability::SetFromJson got no right or left integer");
            return;
        }

        int right = selected["right"].get<int>();
        int left = selected["left"].get<int>();
        if (!IsValidOption(right) || !IsValidOption(left))
        {
            Logger::Error(
                "Error: AapPressAndHoldAirPodModeCapability::SetFromJson got unexpected options: right=%d, left=%d",
                right, left);
            return;
        }

        const bool preferenceIsDisabled =
            _rightOption == DoNotApply && _leftOption == DoNotApply;
        if (preferenceIsDisabled)
        {
            // When enabling the preference through one bud, use its value for
            // both buds so a partial DoNotApply state is never persisted.
            if (right == DoNotApply && left != DoNotApply)
                right = left;
            else if (left == DoNotApply && right != DoNotApply)
                left = right;
        }
        else if (right == DoNotApply || left == DoNotApply)
        {
            // Clearing either bud disables the preference for both buds.
            right = DoNotApply;
            left = DoNotApply;
        }

        const bool changed = _rightOption != right || _leftOption != left;
        _rightOption = right;
        _leftOption = left;
        device.SaveSettingInt(RightSetting, right);
        device.SaveSettingInt(LeftSetting, left);
        SendPreference();

        if (changed)
        {
            Logger::Info("PressAndHoldAirPodMode updated: right=%s, left=%s",
                OptionToString(_rightOption).c_str(), OptionToString(_leftOption).c_str());
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("AapPressAndHoldAirPodModeCapability::SetFromJson set options to right=%s, left=%s",
            OptionToString(_rightOption).c_str(), OptionToString(_leftOption).c_str());
    }
}
