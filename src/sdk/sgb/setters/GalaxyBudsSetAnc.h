// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "sdk/sgb/enums/GalaxyBudsMsgIds.h"
#include "sdk/sgb/enums/GalaxyBudsAnc.h"
#include <vector>

namespace MagicPodsCore
{

    class GalaxyBudsSetAnc
    {
    public:
        const std::vector<unsigned char> Payload;
        const GalaxyBudsMsgIds Id = GalaxyBudsMsgIds::NOISE_CONTROLS;
        explicit GalaxyBudsSetAnc(GalaxyBudsAnc mode) : Payload{static_cast<unsigned char>(mode)} {}
    };
}