// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapNoiseCancellationOneAirPodMode : unsigned char
    {
        On = 0x01,
        Off = 0x02,
    };

    static bool isValidAapNoiseCancellationOneAirPodMode(unsigned char value){
        switch (static_cast<AapNoiseCancellationOneAirPodMode>(value)) {
            case AapNoiseCancellationOneAirPodMode::On:
            case AapNoiseCancellationOneAirPodMode::Off:
                return true;
            default:
                return false;
        }
    }

    static std::string AapNoiseCancellationOneAirPodModeToString(AapNoiseCancellationOneAirPodMode value)
    {
        switch (value)
        {
        case AapNoiseCancellationOneAirPodMode::On:
            return "On";
        case AapNoiseCancellationOneAirPodMode::Off:
            return "Off";
        default:
            return "Unknown value";
        }
    }

}