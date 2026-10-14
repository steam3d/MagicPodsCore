// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapPressAndHoldAirPodMode.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001601050000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      r  l
    // 04 00 04 00 09 00 16 01 05 00 00 Right: Noise control, left: Siri
    //
    // 7 byte:
    // 01 - Noise control for right AirPod
    // 05 - Siri for right AirPod
    //
    // 8 byte:
    // 01 - Noise control for left AirPod
    // 05 - Siri for left AirPod
    // Selects the press-and-hold action for each AirPod
    //
    // It's interesting that Beats reports the currently selected mode but doesn't provide selected noise control options.
    // ---------------------------------------------------------------------------
    class AapSetPressAndHoldAirPodMode : public AapRequest
    {
    private:
        AapPressAndHoldAirPodMode _rightMode;
        AapPressAndHoldAirPodMode _leftMode;

    public:
        AapSetPressAndHoldAirPodMode(
            AapPressAndHoldAirPodMode rightMode,
            AapPressAndHoldAirPodMode leftMode);
        std::vector<unsigned char> Request() const override;
    };
}
