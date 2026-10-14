// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/watchers/AapHearingProtectionStrengthWatcher.h"

namespace MagicPodsCore
{
    // Possibly can be set, but it is not implemented.
    class AapHearingProtectionStrengthCapability : public AapCapability
    {
    private:
        unsigned char option{};
        AapHearingProtectionStrengthWatcher watcher{};
        size_t watcherEventId{};

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapHearingProtectionStrengthCapability(AapDevice& device);
        ~AapHearingProtectionStrengthCapability() override;
        void SetFromJson(const nlohmann::json& json) override {};
    };
}
