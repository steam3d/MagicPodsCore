// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapConversationAwarenessStateWatcher.h"
#include "sdk/aap/enums/AapConversationAwarenessState.h"

namespace MagicPodsCore
{
    class AapConversationAwarenessStateCapability : public AapCapability
    {
    private:
        bool option;        
        AapConversationAwarenessStateWatcher watcher{};
        size_t watcherEventId;
        bool AapConversationAwarenessStateToBoolean(AapConversationAwarenessState mode);        
        
    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapConversationAwarenessStateCapability(AapDevice& device);
        ~AapConversationAwarenessStateCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}