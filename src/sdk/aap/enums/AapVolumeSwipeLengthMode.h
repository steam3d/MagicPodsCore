// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapVolumeSwipeLengthMode : unsigned char
    {
        Default = 0x00,
        Longer = 0x01,
        Longest = 0x02,
    };

    static bool isValidAapVolumeSwipeLengthMode(unsigned char value){
        switch (static_cast<AapVolumeSwipeLengthMode>(value)) {
            case AapVolumeSwipeLengthMode::Default:
            case AapVolumeSwipeLengthMode::Longer:
            case AapVolumeSwipeLengthMode::Longest:
                return true;
            default:
                return false;
        }
    }

    static std::string AapVolumeSwipeLengthModeToString(AapVolumeSwipeLengthMode value)
    {
        switch (value)
        {
        case AapVolumeSwipeLengthMode::Default:
            return "Default";
        case AapVolumeSwipeLengthMode::Longer:
            return "Longer";
        case AapVolumeSwipeLengthMode::Longest:
            return "Longest";
        default:
            return "Unknown value";
        }
    }
}