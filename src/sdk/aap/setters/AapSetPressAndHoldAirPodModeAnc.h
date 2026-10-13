// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001a0f000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 1a 0f 00 00 00 All modes
    //
    // 7 byte:
    // 01 - Off
    // 02 - Noise cancellation
    // 04 - Transparency
    // 08 - Adaptive
    // Bitmask selects the listening modes included when pressing and holding an AirPod
    // assigned to noise control
    // ---------------------------------------------------------------------------
    class AapSetPressAndHoldAirPodModeAnc : public AapRequest
    {
    private:
        unsigned char _modes;

    public:
        explicit AapSetPressAndHoldAirPodModeAnc(unsigned char modes);
        std::vector<unsigned char> Request() const override;
    };
}
