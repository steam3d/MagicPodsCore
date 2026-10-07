// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetMicrophoneOff.h"

#include "sdk/aap/enums/AapCmd.h"

namespace MagicPodsCore
{
    AapSetMicrophoneOff::AapSetMicrophoneOff() : AapRequest{"AapSetMicrophoneOff"}
    {
    }

    std::vector<unsigned char> AapSetMicrophoneOff::Request() const
    {
        return {
            0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Microphone), 0x00,
            0x00, 0x00, 0x02, 0x00, 0x03, 0x01
        };
    }
}
