// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapInitExt.h"
#include "sdk/aap/enums/AapModelIds.h"

namespace MagicPodsCore
{
    AapInitExt::AapInitExt() : AapRequest{"AapInitExt"}
    {
    }

    std::vector<unsigned char> AapInitExt::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, 0x4d, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }

    bool AapInitExt::IsSupported(unsigned short model)
    {
        return model == static_cast<unsigned short>(AapModelIds::airpodspro2) ||
               model == static_cast<unsigned short>(AapModelIds::airpodsprousbc) ||
               model == static_cast<unsigned short>(AapModelIds::airpods4anc);
    }
}