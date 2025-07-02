// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapVolumeSwipeMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002802000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 25 01 00 00 00 On
    // 04 00 04 00 09 00 25 02 00 00 00 Off
    //
    // 7 byte:
    // 01 - On
    // 02 - Off
    // Adjust the volume by swiping up or down on the sensor located on the AirPods Pro stem
    // ---------------------------------------------------------------------------
    class AapSetVolumeSwipe : public AapRequest
    {
    private:
        AapVolumeSwipeMode _mode;

    public:
        AapSetVolumeSwipe(AapVolumeSwipeMode mode);
        std::vector<unsigned char> Request() const override;        
    };
}