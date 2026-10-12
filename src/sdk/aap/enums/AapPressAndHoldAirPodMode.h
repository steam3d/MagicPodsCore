// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapPressAndHoldAirPodMode : unsigned char
    {
        NoiseControl = 0x01,
        Siri = 0x05,
    };

    static bool IsValidAapPressAndHoldAirPodMode(unsigned char value)
    {
        switch (static_cast<AapPressAndHoldAirPodMode>(value))
        {
        case AapPressAndHoldAirPodMode::NoiseControl:
        case AapPressAndHoldAirPodMode::Siri:
            return true;
        default:
            return false;
        }
    }

    static std::string AapPressAndHoldAirPodModeToString(AapPressAndHoldAirPodMode value)
    {
        switch (value)
        {
        case AapPressAndHoldAirPodMode::NoiseControl:
            return "NoiseControl";
        case AapPressAndHoldAirPodMode::Siri:
            return "Siri";
        default:
            return "Unknown value";
        }
    }
}
