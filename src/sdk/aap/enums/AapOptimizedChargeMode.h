// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapOptimizedChargeMode : unsigned char
    {
        On = 0x01,
        Off = 0x02,
    };

    static bool IsValidAapOptimizedChargeMode(unsigned char value)
    {
        switch (static_cast<AapOptimizedChargeMode>(value))
        {
        case AapOptimizedChargeMode::On:
        case AapOptimizedChargeMode::Off:
            return true;
        default:
            return false;
        }
    }

    static std::string AapOptimizedChargeModeToString(AapOptimizedChargeMode value)
    {
        switch (value)
        {
        case AapOptimizedChargeMode::On:
            return "On";
        case AapOptimizedChargeMode::Off:
            return "Off";
        default:
            return "Unknown value";
        }
    }
}
