// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetSleepDetection.h"

#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapSetSleepDetection::AapSetSleepDetection(AapSleepDetectionMode mode)
        : AapRequest{"AapSetSleepDetection"}, _mode{mode}
    {
    }

    std::vector<unsigned char> AapSetSleepDetection::Request() const
    {
        return {
            0x04, 0x00, 0x04, 0x00,
            static_cast<unsigned char>(AapCmd::Settings), 0x00,
            static_cast<unsigned char>(AapCmdSettings::SleepDetection),
            static_cast<unsigned char>(_mode),
            0x00, 0x00, 0x00
        };
    }
}
