// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore {

    enum class GalaxyBudsAnc: unsigned char{
        Off = 0,
        AmbientSound = 2,
        NoiseReduction = 1,
        Adaptive = 3
    };

    static bool isValidGalaxyBudsAncType(unsigned char value){
        switch (static_cast<GalaxyBudsAnc>(value)) {
            case GalaxyBudsAnc::Off:
            case GalaxyBudsAnc::AmbientSound:
            case GalaxyBudsAnc::NoiseReduction:
            case GalaxyBudsAnc::Adaptive:
                return true;
            default:
                return false;
        }
    }

    static std::string GalaxyBudsAncToString(GalaxyBudsAnc value)
    {
        switch (value)
        {
        case GalaxyBudsAnc::Off:
            return "Off";
        case GalaxyBudsAnc::AmbientSound:
            return "AmbientSound";
        case GalaxyBudsAnc::Adaptive:
            return "Adaptive";
        case GalaxyBudsAnc::NoiseReduction:
            return "NoiseReduction";
        default:
            return "Unknown value";
        }
    }
}