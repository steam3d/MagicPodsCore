// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore {

    // maestro_pw.AncState
    enum class PixelBudsAncState: unsigned char{
        Unknown = 0,
        Off = 1,
        Active = 2,
        Aware = 3,
        Adaptive = 4
    };

    static bool isValidPixelBudsAncStateType(unsigned char value){
        switch (static_cast<PixelBudsAncState>(value)) {
            case PixelBudsAncState::Off:
            case PixelBudsAncState::Active:
            case PixelBudsAncState::Aware:
            case PixelBudsAncState::Adaptive:
                return true;
            default:
                return false;
        }
    }

    static std::string PixelBudsAncStateToString(PixelBudsAncState value)
    {
        switch (value)
        {
        case PixelBudsAncState::Off:
            return "Off";
        case PixelBudsAncState::Active:
            return "Active";
        case PixelBudsAncState::Aware:
            return "Aware";
        case PixelBudsAncState::Adaptive:
            return "Adaptive";
        default:
            return "Unknown value";
        }
    }
}
