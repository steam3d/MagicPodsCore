// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#pragma once

#include <cstdint>
#include <span>

namespace MagicPodsCore
{
    class AutomaticGainControl
    {
    private:
        static constexpr float Target = 0.25f;
        static constexpr float MaxGain = 8.0f;
        static constexpr float Attack = 0.02f;
        static constexpr float Release = 0.001f;

        float _envelope = 0.0f;
        float _gain = 1.0f;

    public:
        void Process(std::span<int16_t> samples);
    };
}
