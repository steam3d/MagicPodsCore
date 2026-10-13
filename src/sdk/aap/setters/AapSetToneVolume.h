// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001f32000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 1f 32 00 00 00 50%
    //
    // 7 byte:
    // 0f-64 - Volume from 15 to 100
    // Sets the volume of sound effects played by the AirPods
    // ---------------------------------------------------------------------------
    class AapSetToneVolume : public AapRequest
    {
    private:
        unsigned char _mode{};

    public:
        AapSetToneVolume(unsigned char mode);
        std::vector<unsigned char> Request() const override;
    };
}
