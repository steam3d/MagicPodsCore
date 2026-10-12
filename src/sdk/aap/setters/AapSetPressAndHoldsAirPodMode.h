// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapPressAndHoldsAirPodMode.h"

namespace MagicPodsCore
{
    class AapSetPressAndHoldsAirPodMode : public AapRequest
    {
    private:
        AapPressAndHoldsAirPodMode _rightMode;
        AapPressAndHoldsAirPodMode _leftMode;

    public:
        AapSetPressAndHoldsAirPodMode(
            AapPressAndHoldsAirPodMode rightMode,
            AapPressAndHoldsAirPodMode leftMode);
        std::vector<unsigned char> Request() const override;
    };
}
