// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapConversationAwarenessStateWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapConversationAwarenessStateWatcher::AapConversationAwarenessStateWatcher() : AapWatcher{"AapConversationAwarenessStateWatcher"}
    {
    }

    void AapConversationAwarenessStateWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 10)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::ConversationAwareness))
            return;

        if (data[6] != 0x02) // settings type: I think 0x02 is ConversationAwareness
            return;

        if (!isValidAapConversationAwarenessState(data[9]))
            return;

        AapConversationAwarenessState mode = static_cast<AapConversationAwarenessState>(data[9]);

        Logger::Debug("%s: %s", _tag.c_str(), AapConversationAwarenessStateToString(mode).c_str());
        _event.FireEvent(mode);
    }
}