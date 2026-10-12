// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPressAndHoldsAirPodModeCapability.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/setters/AapSetPressAndHoldsAirPodMode.h"

namespace MagicPodsCore
{
    static constexpr auto RightSetting = "press_and_holds_airpod_mode_right";
    static constexpr auto LeftSetting = "press_and_holds_airpod_mode_left";

    bool AapPressAndHoldsAirPodModeCapability::IsValidOption(int value)
    {
        return value == DoNotApply ||
               (value >= 0 && value <= 0xff &&
                IsValidAapPressAndHoldsAirPodMode(static_cast<unsigned char>(value)));
    }

    std::string AapPressAndHoldsAirPodModeCapability::OptionToString(int value)
    {
        if (value == DoNotApply)
            return "DoNotApply";
        if (value >= 0 && value <= 0xff &&
            IsValidAapPressAndHoldsAirPodMode(static_cast<unsigned char>(value)))
        {
            return AapPressAndHoldsAirPodModeToString(
                static_cast<AapPressAndHoldsAirPodMode>(value));
        }
        return "Unknown value";
    }

    void AapPressAndHoldsAirPodModeCapability::SendPreference()
    {
        // Both bytes must be sent together. Do not overwrite either bud when its
        // current value is unknown and no preference has been configured for it.
        if (_rightOption == DoNotApply || _leftOption == DoNotApply)
            return;

        SendData(AapSetPressAndHoldsAirPodMode(
            static_cast<AapPressAndHoldsAirPodMode>(_rightOption),
            static_cast<AapPressAndHoldsAirPodMode>(_leftOption)));
    }

    nlohmann::json AapPressAndHoldsAirPodModeCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = {
            {"right", _rightOption},
            {"left", _leftOption}
        };
        return bodyJson;
    }

    void AapPressAndHoldsAirPodModeCapability::OnReceivedData(const std::vector<unsigned char>&)
    {
    }

    AapPressAndHoldsAirPodModeCapability::AapPressAndHoldsAirPodModeCapability(AapDevice& device)
        : AapCapability("pressAndHoldsAirPodMode", false, device)
    {
        _clientStateChangedEventId = device.GetClientStateChangedEvent().Subscribe(
            [this](size_t, ClientState state)
            {
                if (state != ClientState::Connected)
                    return;

                _rightOption = DoNotApply;
                _leftOption = DoNotApply;

                const auto rightSetting = this->device.LoadSettingInt(RightSetting);
                if (rightSetting.has_value())
                {
                    const auto value = rightSetting.value();
                    if (value >= DoNotApply && value <= 0xff && IsValidOption(static_cast<int>(value)))
                        _rightOption = static_cast<int>(value);
                    else
                        Logger::Error("PressAndHoldsAirPodMode right setting has unexpected value: %lld",
                            static_cast<long long>(value));
                }

                const auto leftSetting = this->device.LoadSettingInt(LeftSetting);
                if (leftSetting.has_value())
                {
                    const auto value = leftSetting.value();
                    if (value >= DoNotApply && value <= 0xff && IsValidOption(static_cast<int>(value)))
                        _leftOption = static_cast<int>(value);
                    else
                        Logger::Error("PressAndHoldsAirPodMode left setting has unexpected value: %lld",
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
                Logger::Info("PressAndHoldsAirPodMode updated: right=%s, left=%s",
                    OptionToString(_rightOption).c_str(), OptionToString(_leftOption).c_str());
                _onChanged.FireEvent(*this);
            });
    }

    AapPressAndHoldsAirPodModeCapability::~AapPressAndHoldsAirPodModeCapability()
    {
        device.GetClientStateChangedEvent().Unsubscribe(_clientStateChangedEventId);
    }

    bool AapPressAndHoldsAirPodModeCapability::IsSupported(unsigned short model)
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

    void AapPressAndHoldsAirPodModeCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_object())
        {
            Logger::Error("Error: AapPressAndHoldsAirPodModeCapability::SetFromJson got no selected object");
            return;
        }

        const auto& selected = capability["selected"];
        if (!selected.contains("right") || !selected["right"].is_number_integer() ||
            !selected.contains("left") || !selected["left"].is_number_integer())
        {
            Logger::Error("Error: AapPressAndHoldsAirPodModeCapability::SetFromJson got no right or left integer");
            return;
        }

        int right = selected["right"].get<int>();
        int left = selected["left"].get<int>();
        if (!IsValidOption(right) || !IsValidOption(left))
        {
            Logger::Error(
                "Error: AapPressAndHoldsAirPodModeCapability::SetFromJson got unexpected options: right=%d, left=%d",
                right, left);
            return;
        }

        if (right == DoNotApply || left == DoNotApply)
        {
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
            Logger::Info("PressAndHoldsAirPodMode updated: right=%s, left=%s",
                OptionToString(_rightOption).c_str(), OptionToString(_leftOption).c_str());
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("AapPressAndHoldsAirPodModeCapability::SetFromJson set options to right=%s, left=%s",
            OptionToString(_rightOption).c_str(), OptionToString(_leftOption).c_str());
    }
}
