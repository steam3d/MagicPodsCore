// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetPressAndHoldAirPodMode.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapSetPressAndHoldAirPodMode::AapSetPressAndHoldAirPodMode(
        const AapPressAndHoldAirPodModeState& state)
        : AapRequest{"AapSetPressAndHoldAirPodMode"}, _state{state}
    {
    }

    std::vector<unsigned char> AapSetPressAndHoldAirPodMode::Request() const
    {
        const auto& active = _state.GetActive();
        const auto& alternative = _state.GetAlternative();
        return {
            0x04, 0x00, 0x04, 0x00,
            static_cast<unsigned char>(AapCmd::Settings), 0x00,
            static_cast<unsigned char>(AapCmdSettings::PressAndHoldAirPodMode),
            static_cast<unsigned char>(active[0]),
            static_cast<unsigned char>(active[1]),
            static_cast<unsigned char>(alternative[0]),
            static_cast<unsigned char>(alternative[1])
        };
    }
}
