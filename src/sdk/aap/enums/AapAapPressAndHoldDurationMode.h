// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapAapPressAndHoldDurationMode : unsigned char
    {
        Default = 0x00,
        Shorter = 0x01,
        Shortest = 0x02,
    };

    static bool isValidAapAapPressAndHoldDurationMode(unsigned char value){
        switch (static_cast<AapAapPressAndHoldDurationMode>(value)) {
            case AapAapPressAndHoldDurationMode::Default:
            case AapAapPressAndHoldDurationMode::Shorter:
            case AapAapPressAndHoldDurationMode::Shortest:
                return true;
            default:
                return false;
        }
    }

    static std::string AapAapPressAndHoldDurationModeToString(AapAapPressAndHoldDurationMode value)
    {
        switch (value)
        {
        case AapAapPressAndHoldDurationMode::Default:
            return "Default";
        case AapAapPressAndHoldDurationMode::Shorter:
            return "Shorter";
        case AapAapPressAndHoldDurationMode::Shortest:
            return "Shortest";
        default:
            return "Unknown value";
        }
    }
}