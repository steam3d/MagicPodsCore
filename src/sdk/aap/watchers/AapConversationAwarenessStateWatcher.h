// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapConversationAwarenessState.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 040004004b0002000001
    //
    // 0  1  2  3  4  5  6  7  8  9
    //                            s
    // 04 00 04 00 4b 00 02 00 00 01 Start speaking
    //
    // 6 byte:
    // 02 - Conversation awareness state
    //
    // 9 byte:
    // 00 - Unknown
    // 01 - Start speaking
    // 04 - Stop speaking
    // 06 - Stop speaking
    // 08 - Stop speaking
    // 09 - Stop speaking
    // ---------------------------------------------------------------------------
    class AapConversationAwarenessStateWatcher : public AapWatcher
    {
    private:
        Event<AapConversationAwarenessState> _event{};

    public:
        AapConversationAwarenessStateWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapConversationAwarenessState> &GetEvent()
        {
            return _event;
        }
    };

}
