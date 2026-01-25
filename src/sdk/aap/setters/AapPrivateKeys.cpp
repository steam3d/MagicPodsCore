// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPrivateKeys.h"
#include "sdk/aap/enums/AapCmd.h"

namespace MagicPodsCore
{
    AapPrivateKeys::AapPrivateKeys() : AapRequest{"AapPrivateKeys"}
    {
    }
        
    std::vector<unsigned char> AapPrivateKeys::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, 0x30, 0x00, 0x05, 0x00};
    }

}