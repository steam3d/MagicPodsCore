// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore
{
    enum class AapBatteryType : unsigned char
    {
        Single = 0x01,
        Right = 0x02,
        Left = 0x04,
        Case = 0x08,
    };

    static bool isValidAapBatteryType(unsigned char value){
        switch (static_cast<AapBatteryType>(value)) {
            case AapBatteryType::Single:
            case AapBatteryType::Right:
            case AapBatteryType::Left:
            case AapBatteryType::Case:
                return true;
            default:
                return false;
        }
    }

    static std::string AapBatteryTypeToString(AapBatteryType status)
    {
        switch (status)
        {
        case AapBatteryType::Single:
            return "Single";
        case AapBatteryType::Left:
            return "Left  ";
        case AapBatteryType::Right:
            return "Right ";
        case AapBatteryType::Case:
            return "Case  ";
        default:
            return "Unknown";
        }
    }
}