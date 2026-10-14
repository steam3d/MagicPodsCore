// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/watchers/AapOptimizedChargeWatcher.h"

namespace MagicPodsCore
{
    class AapOptimizedChargeCapability : public AapCapability
    {
    private:
        bool option{};
        AapOptimizedChargeWatcher watcher{};
        size_t watcherEventId{};

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapOptimizedChargeCapability(AapDevice& device);
        ~AapOptimizedChargeCapability() override;
        void SetFromJson(const nlohmann::json& json) override;
    };
}
