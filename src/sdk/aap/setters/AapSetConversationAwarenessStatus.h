// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapConversationAwarenessMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002802000000    
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 28 01 00 00 00 On
    // 04 00 04 00 09 00 28 02 00 00 00 Off
    //
    // 7 byte:
    // 01 - On
    // 02 - Off
    // Lower media volume and reduces background noise when you start speaking to other people
    // ---------------------------------------------------------------------------
    class AapSetConversationAwarenessStatus : public AapRequest
    {
    private:
        AapConversationAwarenessMode _mode;

    public:
        AapSetConversationAwarenessStatus(AapConversationAwarenessMode mode);
        std::vector<unsigned char> Request() const override;        
    };
}