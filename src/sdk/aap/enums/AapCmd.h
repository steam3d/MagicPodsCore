// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore
{
    // Byte 4 indicates the packet type
    enum class AapCmd : unsigned char
    {
        Battery = 0x04,
        // Request to change or response that setting in headphones was changed
        Settings = 0x09,
        Notifications = 0x0f,
    };

}