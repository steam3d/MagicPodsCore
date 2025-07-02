// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPersonalizedVolumeWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapPersonalizedVolumeWatcher::AapPersonalizedVolumeWatcher() : AapWatcher{"AapPersonalizedVolumeWatcher"}
    {
    }

    void AapPersonalizedVolumeWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        // min length of the anc packet
        if (data.size() < 10)
            return;

        // packet type must be Settings
        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        // settings type must be ConversationAwareness
        if (data[6] != static_cast<unsigned char>(AapCmdSettings::PersonalizedVolume))
            return;

        if (!isValidAapPersonalizedVolumeMode(data[7]))
            return;

        AapPersonalizedVolumeMode mode = static_cast<AapPersonalizedVolumeMode>(data[7]);

        Logger::Debug("%s: %s", _tag.c_str(), AapPersonalizedVolumeModeToString(mode).c_str());
        _event.FireEvent(mode);
    }
}