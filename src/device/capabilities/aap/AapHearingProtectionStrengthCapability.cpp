// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapHearingProtectionStrengthCapability.h"

namespace MagicPodsCore
{
    nlohmann::json AapHearingProtectionStrengthCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapHearingProtectionStrengthCapability::OnReceivedData(const std::vector<unsigned char>& data)
    {
        watcher.ProcessResponse(data);
    }

    AapHearingProtectionStrengthCapability::AapHearingProtectionStrengthCapability(AapDevice& device)
        : AapCapability("hearingProtectionStrength", true, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t, unsigned char strength)
        {
            if (isAvailable && option == strength)
                return;

            isAvailable = true;
            option = strength;
            Logger::Info("%s updated: %d", name.c_str(), option);
            _onChanged.FireEvent(*this);
        });
    }

    AapHearingProtectionStrengthCapability::~AapHearingProtectionStrengthCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }
}
