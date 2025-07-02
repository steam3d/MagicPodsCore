// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapVolumeSwipeMode : unsigned char
    {
        On = 0x01,
        Off = 0x02,
    };

    static bool isValidAapVolumeSwipeMode(unsigned char value){
        switch (static_cast<AapVolumeSwipeMode>(value)) {
            case AapVolumeSwipeMode::On:
            case AapVolumeSwipeMode::Off:
                return true;
            default:
                return false;
        }
    }

    static std::string AapVolumeSwipeModeToString(AapVolumeSwipeMode value)
    {
        switch (value)
        {
        case AapVolumeSwipeMode::On:
            return "On";
        case AapVolumeSwipeMode::Off:
            return "Off";
        default:
            return "Unknown value";
        }
    }

}