// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PwVarint.h"

namespace MagicPodsCore
{
    std::vector<unsigned char> PwVarint::Encode(uint32_t value)
    {
        std::vector<unsigned char> encoded;
        encoded.reserve(MaxEncodedLength);

        while ((value >> 7) != 0)
        {
            encoded.push_back(static_cast<unsigned char>((value & 0x7F) << 1));
            value >>= 7;
        }
        encoded.push_back(static_cast<unsigned char>(((value & 0x7F) << 1) | 0x01));

        return encoded;
    }

    std::optional<std::pair<uint32_t, size_t>> PwVarint::Decode(const std::vector<unsigned char> &data, size_t offset)
    {
        uint64_t value = 0;

        for (size_t i = offset; i < data.size(); i++)
        {
            const size_t index = i - offset;

            // A uint32_t never takes more than MaxEncodedLength bytes. Anything longer is malformed, and going on would shift by 64 bits or more, which is undefined.
            if (index >= MaxEncodedLength)
                return std::nullopt;

            value |= static_cast<uint64_t>(data[i] >> 1) << (index * 7);

            if (value > UINT32_MAX)
                return std::nullopt;

            if ((data[i] & 0x01) == 0x01)
                return std::make_pair(static_cast<uint32_t>(value), index + 1);
        }

        return std::nullopt;
    }
}
