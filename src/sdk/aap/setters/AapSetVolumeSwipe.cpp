// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetVolumeSwipe.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"
#include "AapInitExt.h"

namespace MagicPodsCore
{
    AapSetVolumeSwipe::AapSetVolumeSwipe(AapVolumeSwipeMode mode) : AapRequest{"AapSetVolumeSwipe"}, _mode{mode}
    {
    }

    std::vector<unsigned char> AapSetVolumeSwipe::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Settings), 0x00, static_cast<unsigned char>(AapCmdSettings::VolumeSwipe), static_cast<unsigned char>(_mode), 0x00, 0x00, 0x00};
    }
}