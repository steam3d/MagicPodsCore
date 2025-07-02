// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapPressAndHoldDurationMode : unsigned char
    {
        Default = 0x00,
        Shorter = 0x01,
        Shortest = 0x02,
    };

    static bool isValidAapPressAndHoldDurationMode(unsigned char value){
        switch (static_cast<AapPressAndHoldDurationMode>(value)) {
            case AapPressAndHoldDurationMode::Default:
            case AapPressAndHoldDurationMode::Shorter:
            case AapPressAndHoldDurationMode::Shortest:
                return true;
            default:
                return false;
        }
    }

    static std::string AapPressAndHoldDurationModeToString(AapPressAndHoldDurationMode value)
    {
        switch (value)
        {
        case AapPressAndHoldDurationMode::Default:
            return "Default";
        case AapPressAndHoldDurationMode::Shorter:
            return "Shorter";
        case AapPressAndHoldDurationMode::Shortest:
            return "Shortest";
        default:
            return "Unknown value";
        }
    }
}