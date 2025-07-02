// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore
{
    // Byte 6 indicates the setting type
    enum class AapCmdSettings : unsigned char
    {
        //Events
        Anc = 0x0d,
        ConversationAwareness = 0x28,
        NoiseCancellationOneAirPod = 0x1b,
        PressAndHoldDuration = 0x18,
    };
}