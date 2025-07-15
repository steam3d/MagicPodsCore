// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapAdaptiveAudioNoiseWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapAdaptiveAudioNoiseWatcher::AapAdaptiveAudioNoiseWatcher() : AapWatcher{"AapAdaptiveAudioNoiseWatcher"}
    {
    }

    void AapAdaptiveAudioNoiseWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 10)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::AdaptiveAudioNoise))
            return;

        unsigned char mode = data[7];

        Logger::Debug("%s: %d", _tag.c_str(), mode);
        _event.FireEvent(mode);
    }
}