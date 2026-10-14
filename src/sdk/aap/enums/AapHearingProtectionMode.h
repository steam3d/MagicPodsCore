// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapHearingProtectionMode : unsigned char
    {
        Unknown = 0x00,
        On = 0x01,
        Off = 0x02,
    };

    static bool IsValidAapHearingProtectionMode(unsigned char value)
    {
        switch (static_cast<AapHearingProtectionMode>(value))
        {
        case AapHearingProtectionMode::Unknown:
        case AapHearingProtectionMode::On:
        case AapHearingProtectionMode::Off:
            return true;
        default:
            return false;
        }
    }

    static std::string AapHearingProtectionModeToString(AapHearingProtectionMode value)
    {
        switch (value)
        {
        case AapHearingProtectionMode::Unknown:
            return "Unknown";
        case AapHearingProtectionMode::On:
            return "On";
        case AapHearingProtectionMode::Off:
            return "Off";
        default:
            return "Unknown value";
        }
    }
}
