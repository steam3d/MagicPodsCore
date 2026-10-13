// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapConversationAwarenessMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002801000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 28 01 00 00 00 On
    // 04 00 04 00 09 00 28 02 00 00 00 Off
    //
    // 7 byte:
    // 01 - On
    // 02 - Off
    // ---------------------------------------------------------------------------
    class AapConversationAwarenessWatcher : public AapWatcher
    {
    private:
        Event<AapConversationAwarenessMode> _event{};

    public:
        AapConversationAwarenessWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapConversationAwarenessMode> &GetEvent()
        {
            return _event;
        }
    };

}
