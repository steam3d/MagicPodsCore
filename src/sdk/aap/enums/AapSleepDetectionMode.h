// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapSleepDetectionMode : unsigned char
    {
        On = 0x01,
        Off = 0x02,
    };

    static bool IsValidAapSleepDetectionMode(unsigned char value)
    {
        switch (static_cast<AapSleepDetectionMode>(value))
        {
        case AapSleepDetectionMode::On:
        case AapSleepDetectionMode::Off:
            return true;
        default:
            return false;
        }
    }

    static std::string AapSleepDetectionModeToString(AapSleepDetectionMode value)
    {
        switch (value)
        {
        case AapSleepDetectionMode::On:
            return "On";
        case AapSleepDetectionMode::Off:
            return "Off";
        default:
            return "Unknown value";
        }
    }
}
