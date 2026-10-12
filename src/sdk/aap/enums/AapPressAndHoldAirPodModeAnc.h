// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore
{
    enum class AapPressAndHoldAirPodModeAnc : unsigned char
    {
        Off = 0x01,
        Anc = 0x02,
        Transparency = 0x04,
        Adaptive = 0x08,
    };
}
