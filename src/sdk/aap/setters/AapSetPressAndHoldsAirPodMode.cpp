// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetPressAndHoldsAirPodMode.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapSetPressAndHoldsAirPodMode::AapSetPressAndHoldsAirPodMode(
        AapPressAndHoldsAirPodMode rightMode,
        AapPressAndHoldsAirPodMode leftMode)
        : AapRequest{"AapSetPressAndHoldsAirPodMode"}, _rightMode{rightMode}, _leftMode{leftMode}
    {
    }

    std::vector<unsigned char> AapSetPressAndHoldsAirPodMode::Request() const
    {
        return std::vector<unsigned char>{
            0x04, 0x00, 0x04, 0x00,
            static_cast<unsigned char>(AapCmd::Settings), 0x00,
            static_cast<unsigned char>(AapCmdSettings::PressAndHoldsAirPodMode),
            static_cast<unsigned char>(_rightMode),
            static_cast<unsigned char>(_leftMode),
            0x00, 0x00
        };
    }
}
