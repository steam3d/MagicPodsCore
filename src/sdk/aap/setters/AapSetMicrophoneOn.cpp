// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetMicrophoneOn.h"

#include "sdk/aap/enums/AapCmd.h"

namespace MagicPodsCore
{
    AapSetMicrophoneOn::AapSetMicrophoneOn() : AapRequest{"AapSetMicrophoneOn"}
    {
    }

    std::vector<unsigned char> AapSetMicrophoneOn::Request() const
    {
        return {
            0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Microphone), 0x00, 0x00, 0x00, 0x09, 0x00,
            0x00, 0x01, 0x82, 0x00, 0x00, 0x00, 0x04, 0x96, 0x00
        };
    }
}
