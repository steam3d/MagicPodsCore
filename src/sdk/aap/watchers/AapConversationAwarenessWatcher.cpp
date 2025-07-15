// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapConversationAwarenessWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapConversationAwarenessWatcher::AapConversationAwarenessWatcher() : AapWatcher{"AapConversationAwarenessWatcher"}
    {
    }

    void AapConversationAwarenessWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 10)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::ConversationAwareness))
            return;

        if (!isValidAapConversationAwarenessMode(data[7]))
            return;

        AapConversationAwarenessMode mode = static_cast<AapConversationAwarenessMode>(data[7]);

        Logger::Debug("%s: %s", _tag.c_str(), AapConversationAwarenessModeToString(mode).c_str());
        _event.FireEvent(mode);
    }
}