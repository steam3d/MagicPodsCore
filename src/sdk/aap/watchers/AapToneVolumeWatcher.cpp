// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapToneVolumeWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapToneVolumeWatcher::AapToneVolumeWatcher() : AapWatcher{"AapToneVolumeWatcher"}
    {
    }

    void AapToneVolumeWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        // min length of the anc packet
        if (data.size() < 10)
            return;

        // packet type must be Settings
        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        // settings type must be ConversationAwareness
        if (data[6] != static_cast<unsigned char>(AapCmdSettings::ToneVolume))
            return;

        unsigned char mode = data[7];

        Logger::Debug("%s: %d", _tag.c_str(), mode);
        _event.FireEvent(mode);
    }
}