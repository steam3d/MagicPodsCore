// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapPrivateKeysWatcher.h"

namespace MagicPodsCore
{
    class AppAnimationCapability : public AapCapability
    {
    private:
        bool option;        
        AapPrivateKeysWatcher watcher{};
        size_t watcherEventId;        
        size_t connectionEventId;        
        
    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AppAnimationCapability(AapDevice& device);
        ~AppAnimationCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}