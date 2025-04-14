// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // Initialize extended features anc adaptive mode for AirPods Pro 2
    // 040004004d000e00000000000000
    //
    // Also working (not recommend to use)
    // 00000400000000000000000000000000
    //
    // Response (Unknown usage)
    // 010004000000010002000500494e0500a54f
    // ---------------------------------------------------------------------------
    class AapInitExt : public AapRequest
    {
    public:
        AapInitExt();
        std::vector<unsigned char> Request() const override;
        static bool IsSupported(unsigned short model);
    };
}