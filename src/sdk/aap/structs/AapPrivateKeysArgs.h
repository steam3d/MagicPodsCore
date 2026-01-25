// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <vector>

namespace MagicPodsCore
{
    struct AapPrivateKeysArgs
    {
        std::vector<unsigned char> IRK;
        std::vector<unsigned char> ENC;
    };
}