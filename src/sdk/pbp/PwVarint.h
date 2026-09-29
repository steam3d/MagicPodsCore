// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <vector>
#include <optional>
#include <utility>
#include <cstdint>
#include <cstddef>

namespace MagicPodsCore
{
    // Variable length integer encoding used for the address field of an HDLC frame.
    // Seven bits are stored per byte, shifted left by one, the lowest bit marks the last byte.
    class PwVarint
    {
    public:
        // A uint32_t holds 32 bits, seven bits per byte gives five bytes at most.
        static constexpr size_t MaxEncodedLength = 5;

        static std::vector<unsigned char> Encode(uint32_t value);
        // Returns the decoded value and the number of consumed bytes, or nothing when the data ends before the last byte, when the value does not fit a uint32_t or when the encoding runs past MaxEncodedLength bytes.
        static std::optional<std::pair<uint32_t, size_t>> Decode(const std::vector<unsigned char> &data, size_t offset = 0);
    };
}
