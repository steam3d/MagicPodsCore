#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class DeviceBatteryType : unsigned char
    {
        Single = 0x01,
        Right = 0x02,
        Left = 0x04,
        Case = 0x08,
    };

    static std::string DeviceBatteryTypeToString(DeviceBatteryType value)
    {
        switch (value)
        {
        case DeviceBatteryType::Single:
            return "Single";
        case DeviceBatteryType::Right:
            return "Right";
        case DeviceBatteryType::Left:
            return "Left";
        case DeviceBatteryType::Case:
            return "Case";
        default:
            return "Unknown value";
        }
    }

}