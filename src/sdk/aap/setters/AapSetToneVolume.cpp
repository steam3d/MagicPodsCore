// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetToneVolume.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapSetToneVolume::AapSetToneVolume(unsigned char mode) : AapRequest{"AapSetToneVolume"}
    {
        if (mode < 0x0f)
            mode = 0x0f;

        if (mode > 0x64)
            mode = 0x64;

        _mode = mode;
    }

    std::vector<unsigned char> AapSetToneVolume::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Settings), 0x00, static_cast<unsigned char>(AapCmdSettings::ToneVolume), static_cast<unsigned char>(_mode), 0x00, 0x00, 0x00};
    }

}