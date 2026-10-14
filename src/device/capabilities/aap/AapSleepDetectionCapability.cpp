// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSleepDetectionCapability.h"

#include "sdk/aap/setters/AapSetSleepDetection.h"

namespace MagicPodsCore
{
    nlohmann::json AapSleepDetectionCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapSleepDetectionCapability::OnReceivedData(const std::vector<unsigned char>& data)
    {
        watcher.ProcessResponse(data);
    }

    AapSleepDetectionCapability::AapSleepDetectionCapability(AapDevice& device)
        : AapCapability("sleepDetection", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t, AapSleepDetectionMode mode)
        {
            const bool newOption = mode == AapSleepDetectionMode::On;
            if (isAvailable && option == newOption)
                return;

            isAvailable = true;
            option = newOption;
            Logger::Info("%s updated: %s", name.c_str(), AapSleepDetectionModeToString(mode).c_str());
            _onChanged.FireEvent(*this);
        });
    }

    AapSleepDetectionCapability::~AapSleepDetectionCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapSleepDetectionCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_boolean())
        {
            Logger::Error("Error: %s::SetFromJson got no value or value is not a boolean", name.c_str());
            return;
        }

        const bool selected = capability["selected"].get<bool>();
        const auto mode = selected ? AapSleepDetectionMode::On : AapSleepDetectionMode::Off;
        SendData(AapSetSleepDetection(mode));

        if (!isAvailable || option != selected)
        {
            isAvailable = true;
            option = selected;
            Logger::Info("%s updated: %s", name.c_str(), AapSleepDetectionModeToString(mode).c_str());
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("%s::SetFromJson set option to %s", name.c_str(),
                      AapSleepDetectionModeToString(mode).c_str());
    }
}
