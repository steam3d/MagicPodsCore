// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    class AapSetPressAndHoldAirPodModeAnc : public AapRequest
    {
    private:
        unsigned char _modes;

    public:
        explicit AapSetPressAndHoldAirPodModeAnc(unsigned char modes);
        std::vector<unsigned char> Request() const override;
    };
}
