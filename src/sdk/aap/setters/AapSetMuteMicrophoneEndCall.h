// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapMuteMicrophoneEndCall.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002400030000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                         m  с
    // 04 00 04 00 09 00 24 21 22 03 00
    // 04 00 04 00 09 00 24 21 23 02 00
    //
    // 8 byte:
    // 22 - double pressing for mute microphone
    // 23 - single pressing for mute microphone
    //
    // 9 byte:
    // 02 - double pressing for end call
    // 03 - single pressing for end call
    //
    // Set the number of clicks to end call or mute microphone
    // When mute microphone set as single pressing then end call must be double pressing
    // When mute microphone set as double pressing then end call must be single pressing
    // ---------------------------------------------------------------------------
    class AapSetMuteMicrophoneEndCall : public AapRequest
    {
    private:
        AapEndCallMode _modeCall;
        AapMuteMicrophoneMode _modeMute;

    public:
        explicit AapSetMuteMicrophoneEndCall(AapEndCallMode mode);
        explicit AapSetMuteMicrophoneEndCall(AapMuteMicrophoneMode mode);
        std::vector<unsigned char> Request() const override;        
    };
}