// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapAapPressAndHoldDurationMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001800000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 18 00 00 00 00 default
    // 04 00 04 00 09 00 18 01 00 00 00 Shorter
    // 04 00 04 00 09 00 18 02 00 00 00 Shortest
    //
    // 7 byte:
    // 00 - default
    // 01 - Shorter
    // 02 - Shortest
    // Adjust the duration required to press and hold on your AirPods
    // ---------------------------------------------------------------------------
    class AapSetPressAndHoldDuration : public AapRequest
    {
    private:
        AapAapPressAndHoldDurationMode _mode;

    public:
        AapSetPressAndHoldDuration(AapAapPressAndHoldDurationMode mode);
        std::vector<unsigned char> Request() const override;        
    };
}