// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapPressAndHoldsAirPodMode : unsigned char
    {
        NoiseControl = 0x01,
        Siri = 0x05,
    };

    static bool IsValidAapPressAndHoldsAirPodMode(unsigned char value)
    {
        switch (static_cast<AapPressAndHoldsAirPodMode>(value))
        {
        case AapPressAndHoldsAirPodMode::NoiseControl:
        case AapPressAndHoldsAirPodMode::Siri:
            return true;
        default:
            return false;
        }
    }

    static std::string AapPressAndHoldsAirPodModeToString(AapPressAndHoldsAirPodMode value)
    {
        switch (value)
        {
        case AapPressAndHoldsAirPodMode::NoiseControl:
            return "NoiseControl";
        case AapPressAndHoldsAirPodMode::Siri:
            return "Siri";
        default:
            return "Unknown value";
        }
    }
}
