// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapVolumeSwipeWatcher.h"

namespace MagicPodsCore
{
    class AapVolumeSwipeCapability : public AapCapability
    {
    private:
        bool option;        
        AapVolumeSwipeWatcher watcher{};
        size_t watcherEventId;        
        
    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapVolumeSwipeCapability(AapDevice& device);
        ~AapVolumeSwipeCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}