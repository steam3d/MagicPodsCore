// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapCustomEqState : unsigned char
    {
        Disabled = 0x01,
        Enabled = 0x02,
    };

    static bool isValidAapCustomEqState(unsigned char value)
    {
        switch (static_cast<AapCustomEqState>(value))
        {
        case AapCustomEqState::Disabled:
        case AapCustomEqState::Enabled:
            return true;
        default:
            return false;
        }
    }

    static std::string AapCustomEqStateToString(AapCustomEqState value)
    {
        switch (value)
        {
        case AapCustomEqState::Disabled:
            return "Disabled";
        case AapCustomEqState::Enabled:
            return "Enabled";
        default:
            return "Unknown value";
        }
    }
}
