// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapMuteMicrophoneEndCall.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002421220300
    // 0400040009002420030000 Custom response
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                         m  c
    // 04 00 04 00 09 00 24 21 22 03 00
    //
    // 7 byte:
    // 21 - Standard response
    // 20 - Custom response, possibly newer firmware or H2 chip
    //
    // Standard response, 8 byte:
    // 22 - Double pressing for mute microphone
    // 23 - Single pressing for mute microphone
    //
    // Standard response, 9 byte:
    // 02 - Double pressing for end call
    // 03 - Single pressing for end call
    //
    // Custom response, 8 byte:
    // 02 - Double pressing for mute microphone, single pressing for end call
    // 03 - Single pressing for mute microphone, double pressing for end call
    // ---------------------------------------------------------------------------
    class AapMuteMicrophoneEndCallWatcher: public AapWatcher
    {
    private:
        Event<AapMuteMicrophoneEndCallMode> _event{};

    public:
        AapMuteMicrophoneEndCallWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapMuteMicrophoneEndCallMode> &GetEvent()
        {
            return _event;
        }
    };
}
