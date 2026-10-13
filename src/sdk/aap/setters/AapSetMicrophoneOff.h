// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 040004005800000002000301
    //
    // Stops the high-resolution microphone audio stream.
    // ---------------------------------------------------------------------------
    class AapSetMicrophoneOff final : public AapRequest
    {
    public:
        AapSetMicrophoneOff();
        std::vector<unsigned char> Request() const override;
    };
}
