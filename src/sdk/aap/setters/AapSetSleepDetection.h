// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapSleepDetectionMode.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009003501000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 35 01 00 00 00 On
    // 04 00 04 00 09 00 35 02 00 00 00 Off
    //
    // 7 byte:
    // 01 - On
    // 02 - Off
    // Enables or disables sleep detection.
    // ---------------------------------------------------------------------------
    class AapSetSleepDetection : public AapRequest
    {
    private:
        AapSleepDetectionMode _mode;

    public:
        explicit AapSetSleepDetection(AapSleepDetectionMode mode);
        std::vector<unsigned char> Request() const override;
    };
}
