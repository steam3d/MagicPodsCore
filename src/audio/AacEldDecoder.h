// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#pragma once

#include <cstdint>
#include <span>
#include <vector>

struct AVCodecContext;
struct AVFrame;
struct AVPacket;

namespace MagicPodsCore
{
    class AacEldDecoder
    {
    private:
        static constexpr size_t MaxInputLength = 512;

        AVCodecContext* _context{};
        AVPacket* _packet{};
        AVFrame* _frame{};
        std::vector<unsigned char> _inputBuffer{};

        void Free();

    public:
        static constexpr uint32_t SampleRate = 64000;
        static constexpr size_t FrameSamples = 480;
        static constexpr uint8_t Channels = 1;

        AacEldDecoder();
        ~AacEldDecoder();

        AacEldDecoder(const AacEldDecoder&) = delete;
        AacEldDecoder& operator=(const AacEldDecoder&) = delete;

        bool IsOpen() const;
        bool Decode(std::span<const unsigned char> accessUnit, std::vector<int16_t>& output);
    };
}
