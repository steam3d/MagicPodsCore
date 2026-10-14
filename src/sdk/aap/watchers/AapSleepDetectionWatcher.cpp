// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSleepDetectionWatcher.h"

#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapSleepDetectionWatcher::AapSleepDetectionWatcher() : AapWatcher{"AapSleepDetectionWatcher"}
    {
    }

    void AapSleepDetectionWatcher::ProcessResponse(const std::vector<unsigned char>& data)
    {
        if (data.size() < 8)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::SleepDetection))
            return;

        if (!IsValidAapSleepDetectionMode(data[7]))
            return;

        const auto mode = static_cast<AapSleepDetectionMode>(data[7]);
        Logger::Debug("%s: %s", _tag.c_str(), AapSleepDetectionModeToString(mode).c_str());
        _event.FireEvent(mode);
    }
}
