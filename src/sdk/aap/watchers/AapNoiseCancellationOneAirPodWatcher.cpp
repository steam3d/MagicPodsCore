// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapNoiseCancellationOneAirPodWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapNoiseCancellationOneAirPodWatcher::AapNoiseCancellationOneAirPodWatcher() : AapWatcher{"AapNoiseCancellationOneAirPodWatcher"}
    {
    }

    void AapNoiseCancellationOneAirPodWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 10)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::NoiseCancellationOneAirPod))
            return;

        if (!isValidAapNoiseCancellationOneAirPodMode(data[7]))
            return;

        AapNoiseCancellationOneAirPodMode mode = static_cast<AapNoiseCancellationOneAirPodMode>(data[7]);

        Logger::Debug("%s: %s", _tag.c_str(), AapNoiseCancellationOneAirPodModeToString(mode).c_str());
        _event.FireEvent(mode);
    }
}