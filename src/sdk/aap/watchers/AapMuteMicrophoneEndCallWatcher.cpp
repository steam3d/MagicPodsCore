// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapMuteMicrophoneEndCallWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapMuteMicrophoneEndCallWatcher::AapMuteMicrophoneEndCallWatcher() : AapWatcher{"AapMuteMicrophoneEndCallWatcher"}
    {
    }

    void AapMuteMicrophoneEndCallWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 10)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::EndCall))
            return;

        if (data[7] == 0x21){

            if (!isValidAapMuteMicrophoneMode(data[8]))
                return;

            if (!isValidAapEndCallMode(data[9]))
                return;

            AapMuteMicrophoneEndCallMode mode;
            mode.muteMicrophoneMode = static_cast<AapMuteMicrophoneMode>(data[8]);;
            mode.endCallMode = static_cast<AapEndCallMode>(data[9]);

            Logger::Debug("%s: Mute mic:%s/End call:%s", _tag.c_str(), AapMuteMicrophoneModeToString(mode.muteMicrophoneMode).c_str(), AapEndCallModeToString(mode.endCallMode).c_str());
            _event.FireEvent(mode);
        }
        //Custom possible AirPods firmware update or H2 chip
        else if (data[7] == 0x20){

            if (data[8] != 0x02 && data[8] != 0x03)
                return;

            AapMuteMicrophoneEndCallMode mode;
            if (data[8] == 0x03){
                mode.endCallMode = AapEndCallMode::DoublePress;
                mode.muteMicrophoneMode = AapMuteMicrophoneMode::SinglePress;
            }

            if (data[8] == 0x02){
                mode.endCallMode = AapEndCallMode::SinglePress;
                mode.muteMicrophoneMode = AapMuteMicrophoneMode::DoublePress;
            }

            Logger::Debug("%s: Mute mic:%s/End call:%s", _tag.c_str(), AapMuteMicrophoneModeToString(mode.muteMicrophoneMode).c_str(), AapEndCallModeToString(mode.endCallMode).c_str());
            _event.FireEvent(mode);
        }

    }
}