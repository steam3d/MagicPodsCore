// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapDigitalCrownMode : unsigned char
    {
        BackToFront = 0x01,
        FrontToBack = 0x02,
    };

    static bool IsValidAapDigitalCrownMode(unsigned char value)
    {
        switch (static_cast<AapDigitalCrownMode>(value))
        {
        case AapDigitalCrownMode::BackToFront:
        case AapDigitalCrownMode::FrontToBack:
            return true;
        default:
            return false;
        }
    }

    static std::string AapDigitalCrownModeToString(AapDigitalCrownMode value)
    {
        switch (value)
        {
        case AapDigitalCrownMode::BackToFront:
            return "BackToFront";
        case AapDigitalCrownMode::FrontToBack:
            return "FrontToBack";
        default:
            return "Unknown value";
        }
    }
}
