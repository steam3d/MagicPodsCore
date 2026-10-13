// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 04000400580000000900000182000000049600
    //
    // Starts the high-resolution microphone audio stream.
    // ---------------------------------------------------------------------------
    class AapSetMicrophoneOn final : public AapRequest
    {
    public:
        AapSetMicrophoneOn();
        std::vector<unsigned char> Request() const override;
    };
}
