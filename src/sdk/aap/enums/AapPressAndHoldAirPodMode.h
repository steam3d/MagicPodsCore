// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapPressAndHoldAirPodMode : unsigned char
    {
        Siri = 0x01,
        NoiseControl = 0x05,
        VolumeUp = 0x06,
        VolumeDown = 0x07,
    };

    static bool IsValidAapPressAndHoldAirPodMode(unsigned char value)
    {
        switch (static_cast<AapPressAndHoldAirPodMode>(value))
        {
        case AapPressAndHoldAirPodMode::Siri:
        case AapPressAndHoldAirPodMode::NoiseControl:
        case AapPressAndHoldAirPodMode::VolumeUp:
        case AapPressAndHoldAirPodMode::VolumeDown:
            return true;
        default:
            return false;
        }
    }

    static std::string AapPressAndHoldAirPodModeToString(AapPressAndHoldAirPodMode value)
    {
        switch (value)
        {
        case AapPressAndHoldAirPodMode::Siri:
            return "Siri";
        case AapPressAndHoldAirPodMode::NoiseControl:
            return "NoiseControl";
        case AapPressAndHoldAirPodMode::VolumeUp:
            return "VolumeUp";
        case AapPressAndHoldAirPodMode::VolumeDown:
            return "VolumeDown";
        default:
            return "Unknown value";
        }
    }
}
