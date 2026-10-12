// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapPressAndHoldAirPodMode.h"

namespace MagicPodsCore
{
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
