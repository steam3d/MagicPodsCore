// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#include "AutomaticGainControl.h"

#include <algorithm>
#include <cmath>

namespace MagicPodsCore
{
    void AutomaticGainControl::Process(std::span<int16_t> samples)
    {
        for (auto& sample : samples)
        {
            const float input = static_cast<float>(sample) / 32768.0f;
            const float level = std::abs(input);
            const float smoothing = level > _envelope ? Attack : Release;
            _envelope += (level - _envelope) * smoothing;

            const float desiredGain = std::min(Target / std::max(_envelope, 0.0001f), MaxGain);
            _gain += (desiredGain - _gain) * 0.01f;

            const float output = std::tanh(input * _gain);
            sample = static_cast<int16_t>(output * 32767.0f);
        }
    }
}
