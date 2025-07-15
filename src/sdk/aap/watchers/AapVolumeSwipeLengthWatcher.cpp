// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapVolumeSwipeLengthWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapVolumeSwipeLengthWatcher::AapVolumeSwipeLengthWatcher() : AapWatcher{"AapVolumeSwipeLengthWatcher"}
    {
    }

    void AapVolumeSwipeLengthWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 10)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::VolumeSwipeLength))
            return;

        if (!isValidAapVolumeSwipeLengthMode(data[7]))
            return;

        AapVolumeSwipeLengthMode mode = static_cast<AapVolumeSwipeLengthMode>(data[7]);

        Logger::Debug("%s: %s", _tag.c_str(), AapVolumeSwipeLengthModeToString(mode).c_str());
        _event.FireEvent(mode);
    }
}