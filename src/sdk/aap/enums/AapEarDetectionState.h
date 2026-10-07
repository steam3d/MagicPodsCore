// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class AapEarDetectionState
    {
        InEar = 0x00,
        OutEar = 0x01,
        InCase = 0x02,
        /// <summary>
        /// Unknown should be tested
        /// </summary>
        _InClosedCase = 0x03,
    };

    static bool isValidAapEarDetectionState(unsigned char value){
        switch (static_cast<AapEarDetectionState>(value)) {
            case AapEarDetectionState::InEar:
            case AapEarDetectionState::OutEar:
            case AapEarDetectionState::InCase:
            case AapEarDetectionState::_InClosedCase:
                return true;
            default:
                return false;
        }
    }
    static std::string AapEarDetectionStateToString(AapEarDetectionState value)
    {
        switch (value)
        {
        case AapEarDetectionState::InEar:
            return "InEar";
        case AapEarDetectionState::OutEar:
            return "OutEar";
        case AapEarDetectionState::InCase:
            return "InCase";
        case AapEarDetectionState::_InClosedCase:
            return "_InClosedCase";
        default:
            return "Unknown value";
        }
    }
}