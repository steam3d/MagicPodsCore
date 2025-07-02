// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapConversationAwarenessWatcher.h"

namespace MagicPodsCore
{
    class AapConversationAwarenessCapability : public AapCapability
    {
    private:
        bool option;        
        AapConversationAwarenessWatcher watcher{};
        size_t watcherEventId;        
        
    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapConversationAwarenessCapability(AapDevice& device);
        ~AapConversationAwarenessCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}