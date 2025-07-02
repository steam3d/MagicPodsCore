// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapPersonalizedVolumeMode : unsigned char
    {
        On = 0x01,
        Off = 0x02,
    };

    static bool isValidAapPersonalizedVolumeMode(unsigned char value){
        switch (static_cast<AapPersonalizedVolumeMode>(value)) {
            case AapPersonalizedVolumeMode::On:
            case AapPersonalizedVolumeMode::Off:
                return true;
            default:
                return false;
        }
    }

    static std::string AapPersonalizedVolumeModeToString(AapPersonalizedVolumeMode value)
    {
        switch (value)
        {
        case AapPersonalizedVolumeMode::On:
            return "On";
        case AapPersonalizedVolumeMode::Off:
            return "Off";
        default:
            return "Unknown value";
        }
    }

}