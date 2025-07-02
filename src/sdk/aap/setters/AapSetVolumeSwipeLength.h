// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapVolumeSwipeLengthMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002300000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 23 00 00 00 00 default
    // 04 00 04 00 09 00 23 01 00 00 00 Shorter
    // 04 00 04 00 09 00 23 02 00 00 00 Shortest
    //
    // 8 byte:
    // 00 - default
    // 01 - Shorter
    // 02 - Shortest
    // To prevent unintended volume adjustments, select preferred waiting time between swipes
    // ---------------------------------------------------------------------------
    class AapSetVolumeSwipeLength : public AapRequest
    {
    private:
        AapVolumeSwipeLengthMode _mode;

    public:
        AapSetVolumeSwipeLength(AapVolumeSwipeLengthMode mode);
        std::vector<unsigned char> Request() const override;        
    };
}