// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPressAndHoldAirPodModeCapability.h"
#include "sdk/aap/setters/AapSetPressAndHoldAirPodMode.h"

namespace MagicPodsCore
{
    nlohmann::json AapPressAndHoldAirPodModeCapability::CreateJsonBody()
    {
        const auto& active = option->GetActive();
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = {
            {"right", active[0]},
            {"left", active[1]}
        };
        bodyJson["options"] = option->IsVolumeSupported()
            ? nlohmann::json{
                AapPressAndHoldAirPodMode::Siri,
                AapPressAndHoldAirPodMode::NoiseControl,
                AapPressAndHoldAirPodMode::VolumeDown,
                AapPressAndHoldAirPodMode::VolumeUp}
            : nlohmann::json{
                AapPressAndHoldAirPodMode::Siri,
                AapPressAndHoldAirPodMode::NoiseControl};
        return bodyJson;
    }

    void AapPressAndHoldAirPodModeCapability::OnReceivedData(const std::vector<unsigned char>& data)
    {
        watcher.ProcessResponse(data);
    }

    AapPressAndHoldAirPodModeCapability::AapPressAndHoldAirPodModeCapability(AapDevice& device) : AapCapability("pressAndHoldAirPodMode", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t, const AapPressAndHoldAirPodModeState& mode)
        {
            if (!isAvailable)
            {
                isAvailable = true;
                option = mode;
                _onChanged.FireEvent(*this);
            }
            else if (option.value() != mode)
            {
                option = mode;
                _onChanged.FireEvent(*this);
            }
        });
    }

    AapPressAndHoldAirPodModeCapability::~AapPressAndHoldAirPodModeCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapPressAndHoldAirPodModeCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        if (!isAvailable || !option.has_value())
        {
            Logger::Error("Error: %s::SetFromJson got no state from headphones", name.c_str());
            return;
        }

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability.at("selected").is_object())
        {
            Logger::Error("Error: %s::SetFromJson got no selected object", name.c_str());
            return;
        }

        const auto& selected = capability.at("selected");
        if (selected.size() != 1 ||
            (!selected.contains("right") && !selected.contains("left")) ||
            !selected.begin().value().is_number_integer())
        {
            Logger::Error("Error: %s::SetFromJson expects exactly one right or left integer", name.c_str());
            return;
        }

        const bool isRight = selected.contains("right");
        const int value = selected.begin().value().get<int>();
        const auto newOption = isRight
            ? option->SetRight(value)
            : option->SetLeft(value);

        if (newOption == option.value())
            return;

        SendData(AapSetPressAndHoldAirPodMode(newOption));

        // Manually notify because the headphones do not report the changed setting.
        option = newOption;
        _onChanged.FireEvent(*this);
    }
}
