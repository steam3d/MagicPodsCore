#pragma once

#include <string>

namespace MagicPodsCore
{
    //Universal parameters for UI
    enum class DeviceAncModes : unsigned char
    {
        Off = 1,
        Transparency = 2,
        Adaptive = 4,
        WindCancellation = 8,
        NoiseCancellation = 16,
    };

    static bool isValidDeviceAncModesType(unsigned char value){
        switch (static_cast<DeviceAncModes>(value)) {
            case DeviceAncModes::Off:
            case DeviceAncModes::Transparency:
            case DeviceAncModes::Adaptive:
            case DeviceAncModes::WindCancellation:
            case DeviceAncModes::NoiseCancellation:
                return true;
            default:
                return false;
        }
    }

    static std::string DeviceAncModesToString(DeviceAncModes value)
    {
        switch (value)
        {
        case DeviceAncModes::Off:
            return "Off";
        case DeviceAncModes::Transparency:
            return "Transparency";
        case DeviceAncModes::Adaptive:
            return "Adaptive";
        case DeviceAncModes::WindCancellation:
            return "WindCancellation";
        case DeviceAncModes::NoiseCancellation:
            return "NoiseCancellation";
        default:
            return "Unknown value";
        }
    }
}