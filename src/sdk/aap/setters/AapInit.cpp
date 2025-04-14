// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapInit.h"

namespace MagicPodsCore
{
    AapInit::AapInit() : AapRequest{"AapInit"}
    {
    }

    std::vector<unsigned char> AapInit::Request() const
    {
        return std::vector<unsigned char>{0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }
}