// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002e32000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 2e 32 00 00 00 50%
    //
    // 7 byte:
    // 00-64 - Noise level from 0 to 100
    // Controls how much external noise Adaptive Audio allows
    // 0 allows more noise, 100 allows less noise
    // ---------------------------------------------------------------------------
    class AapSetAdaptiveAudioNoise : public AapRequest
    {
    private:
        unsigned char _mode{};

    public:
        AapSetAdaptiveAudioNoise(unsigned char mode);
        std::vector<unsigned char> Request() const override;
    };
}
