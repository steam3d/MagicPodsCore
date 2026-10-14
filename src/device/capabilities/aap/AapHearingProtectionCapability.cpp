// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapHearingProtectionCapability.h"

#include "sdk/aap/setters/AapSetHearingProtection.h"

namespace MagicPodsCore
{
    nlohmann::json AapHearingProtectionCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapHearingProtectionCapability::OnReceivedData(const std::vector<unsigned char>& data)
    {
        watcher.ProcessResponse(data);
    }

    AapHearingProtectionCapability::AapHearingProtectionCapability(AapDevice& device)
        : AapCapability("hearingProtection", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t, AapHearingProtectionMode mode)
        {
            const bool newOption = mode == AapHearingProtectionMode::On;
            if (isAvailable && option == newOption)
                return;

            isAvailable = true;
            option = newOption;
            Logger::Info("%s updated: %s", name.c_str(), AapHearingProtectionModeToString(mode).c_str());
            _onChanged.FireEvent(*this);
        });
    }

    AapHearingProtectionCapability::~AapHearingProtectionCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapHearingProtectionCapability::SetFromJson(const nlohmann::json& json)
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
        const auto mode = selected ? AapHearingProtectionMode::On : AapHearingProtectionMode::Off;
        SendData(AapSetHearingProtection(mode));

        if (!isAvailable || option != selected)
        {
            isAvailable = true;
            option = selected;
            Logger::Info("%s updated: %s", name.c_str(), AapHearingProtectionModeToString(mode).c_str());
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("%s::SetFromJson set option to %s", name.c_str(),
                      AapHearingProtectionModeToString(mode).c_str());
    }
}
