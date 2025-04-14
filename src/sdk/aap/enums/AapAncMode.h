// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the anc mode
    enum class AapAncMode
    {
        Off = 0x01,
        Anc = 0x02,
        Transparency = 0x03,
        Adaptive = 0x04,
    };

    static std::string AapAncModeToString(AapAncMode value)
    {
        switch (value)
        {
        case AapAncMode::Off:
            return "Off";
        case AapAncMode::Transparency:
            return "Transparency";
        case AapAncMode::Adaptive:
            return "Adaptive";
        case AapAncMode::Anc:
            return "WindCancellation";
        default:
            return "Unknown value";
        }
    }
}