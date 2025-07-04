// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetMuteMicrophoneEndCall.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"
#include "AapInitExt.h"

namespace MagicPodsCore
{
    AapSetMuteMicrophoneEndCall::AapSetMuteMicrophoneEndCall(AapEndCallMode mode) : AapRequest{"AapSetMuteMicrophoneEndCall"}
    {
        _modeCall = mode;
        _modeMute = mode == AapEndCallMode::SinglePress ? AapMuteMicrophoneMode::DoublePress : AapMuteMicrophoneMode::SinglePress;
    }

    AapSetMuteMicrophoneEndCall::AapSetMuteMicrophoneEndCall(AapMuteMicrophoneMode mode) : AapRequest{"AapSetMuteMicrophoneEndCall"}
    {
        _modeMute = mode;                        
        _modeCall = mode == AapMuteMicrophoneMode::SinglePress ? AapEndCallMode::DoublePress : AapEndCallMode::SinglePress;
    }

    std::vector<unsigned char> AapSetMuteMicrophoneEndCall::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Settings), 0x00, static_cast<unsigned char>(AapCmdSettings::EndCall), 0x21, static_cast<unsigned char>(_modeMute), static_cast<unsigned char>(_modeCall), 0x00};
    }
}