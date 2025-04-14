// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // Initialization handshake with aap service
    // 00000400010002000000000000000000
    //
    // Also working (not recommend to use)
    // 00000400000000000000000000000000
    //
    // Responce (Unknown ussage)
    // 010004000000010002000500494e0500a54f
    // ---------------------------------------------------------------------------
    class AapInit : public AapRequest
    {
    public:
        AapInit();
        std::vector<unsigned char> Request() const override;
    };
}