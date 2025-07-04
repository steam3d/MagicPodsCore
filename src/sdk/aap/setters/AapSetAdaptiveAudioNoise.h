// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    class AapSetAdaptiveAudioNoise : public AapRequest
    {
    private:
        unsigned char _mode{};

    public:
        AapSetAdaptiveAudioNoise(unsigned char mode);
        std::vector<unsigned char> Request() const override;
    };
}