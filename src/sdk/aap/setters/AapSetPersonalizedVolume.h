// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapPersonalizedVolumeMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002602000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 26 01 00 00 00 On
    // 04 00 04 00 09 00 26 02 00 00 00 Off
    //
    // 7 byte:
    // 01 - On
    // 02 - Off
    // Adjust the volume of media in response to your environment
    // ---------------------------------------------------------------------------
    class AapSetPersonalizedVolume : public AapRequest
    {
    private:
        AapPersonalizedVolumeMode _mode;

    public:
        AapSetPersonalizedVolume(AapPersonalizedVolumeMode mode);
        std::vector<unsigned char> Request() const override;        
    };
}