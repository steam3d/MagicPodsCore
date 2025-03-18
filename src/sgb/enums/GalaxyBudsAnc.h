#pragma once

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
        }
    }
}