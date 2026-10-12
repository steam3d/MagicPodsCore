// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapAncOffMode : unsigned char
    {
        Enabled = 0x01,
        Disabled = 0x02,
    };

    static bool IsValidAapAncOffMode(unsigned char value)
    {
        switch (static_cast<AapAncOffMode>(value))
        {
        case AapAncOffMode::Enabled:
        case AapAncOffMode::Disabled:
            return true;
        default:
            return false;
        }
    }

    static std::string AapAncOffModeToString(AapAncOffMode value)
    {
        switch (value)
        {
        case AapAncOffMode::Enabled:
            return "Enabled";
        case AapAncOffMode::Disabled:
            return "Disabled";
        default:
            return "Unknown value";
        }
    }
}
