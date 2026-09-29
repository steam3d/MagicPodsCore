// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <vector>
#include <optional>
#include <cstdint>
#include <cstddef>

namespace MagicPodsCore
{
    struct HdlcFrame
    {
        uint32_t Address{};
        unsigned char Control{};
        std::vector<unsigned char> Data{};
    };

    // High level data link control framing as used by Pigweed RPC:
    // 0x7E | address (PwVarint) | control | data | CRC-32 (little endian) | 0x7E
    // The checksum is calculated over the unescaped address, control and data bytes.
    // 0x7E and 0x7D are escaped as 0x7D followed by the byte xored with 0x20.
    class Hdlc
    {
    private:
        static inline constexpr unsigned char frameFlag = 0x7E;
        static inline constexpr unsigned char escapeFlag = 0x7D;
        static inline constexpr unsigned char escapeMask = 0x20;
        static inline constexpr size_t minFrameSize = 6;
        static inline constexpr size_t maxFrameSize = 4096;

        std::vector<unsigned char> buffer{};
        bool isInFrame = false;
        bool isEscaped = false;
        bool isCorrupt = false;

        std::optional<HdlcFrame> ExtractBuffered();

    public:
        // Control byte of the frames carrying a Pigweed RPC packet
        static inline constexpr unsigned char RpcControl = 0x03;

        static std::vector<unsigned char> Encode(const HdlcFrame &frame);
        // The decoder keeps its state between calls, a frame may be split over several chunks.
        std::vector<HdlcFrame> Decode(const std::vector<unsigned char> &chunk);
        void Reset();
    };
}
