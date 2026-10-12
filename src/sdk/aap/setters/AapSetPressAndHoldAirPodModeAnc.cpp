// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetPressAndHoldAirPodModeAnc.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapSetPressAndHoldAirPodModeAnc::AapSetPressAndHoldAirPodModeAnc(unsigned char modes)
        : AapRequest{"AapSetPressAndHoldAirPodModeAnc"}, _modes{modes}
    {
    }

    std::vector<unsigned char> AapSetPressAndHoldAirPodModeAnc::Request() const
    {
        return std::vector<unsigned char>{
            0x04, 0x00, 0x04, 0x00,
            static_cast<unsigned char>(AapCmd::Settings), 0x00,
            static_cast<unsigned char>(AapCmdSettings::PressAndHoldAirPodModeAnc),
            _modes, 0x00, 0x00, 0x00
        };
    }
}
