// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetAdaptiveAudioNoise.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapSetAdaptiveAudioNoise::AapSetAdaptiveAudioNoise(unsigned char mode) : AapRequest{"AapSetAdaptiveAudioNoise"}
    {
        if (mode > 0x64)
            mode = 0x64;

        _mode = mode;
    }

    std::vector<unsigned char> AapSetAdaptiveAudioNoise::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Settings), 0x00, static_cast<unsigned char>(AapCmdSettings::AdaptiveAudioNoise), static_cast<unsigned char>(_mode), 0x00, 0x00, 0x00};
    }

}