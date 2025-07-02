// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapPressSpeedMode : unsigned char
    {
        Default = 0x00,
        Slower = 0x01,
        Slowest = 0x02,
    };

    static bool isValidAapPressSpeedMode(unsigned char value){
        switch (static_cast<AapPressSpeedMode>(value)) {
            case AapPressSpeedMode::Default:
            case AapPressSpeedMode::Slower:
            case AapPressSpeedMode::Slowest:
                return true;
            default:
                return false;
        }
    }

    static std::string AapPressSpeedModeToString(AapPressSpeedMode value)
    {
        switch (value)
        {
        case AapPressSpeedMode::Default:
            return "Default";
        case AapPressSpeedMode::Slower:
            return "Slower";
        case AapPressSpeedMode::Slowest:
            return "Slowest";
        default:
            return "Unknown value";
        }
    }
}