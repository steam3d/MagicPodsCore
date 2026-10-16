// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/structs/AapPressAndHoldAirPodModeState.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001605050706
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      r  l  ar al
    // 04 00 04 00 09 00 16 05 05 07 06 Active: Noise control for both, alternative: Volume down/up
    //
    // 7-8 bytes:
    // Active mode for the right and left earbud.
    //
    // 9-10 bytes:
    // Alternative mode for the right and left earbud.
    //
    // 01 - Siri
    // 05 - Noise control
    // 06 - Volume up
    // 07 - Volume down
    //
    // Both pairs must contain valid modes. Volume control uses a complementary
    // 06/07 or 07/06 pair.
    //
    // This packet is confirmed for compatible Beats models.
    // AirPods behavior requires additional research.
    // ---------------------------------------------------------------------------
    class AapSetPressAndHoldAirPodMode : public AapRequest
    {
    private:
        AapPressAndHoldAirPodModeState _state;

    public:
        explicit AapSetPressAndHoldAirPodMode(const AapPressAndHoldAirPodModeState& state);
        std::vector<unsigned char> Request() const override;
    };
}
