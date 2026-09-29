// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "Crc32.h"

namespace MagicPodsCore
{
    uint32_t Crc32::Checksum(const std::vector<unsigned char> &data)
    {
        return Checksum(data.data(), data.size());
    }

    uint32_t Crc32::Checksum(const unsigned char *data, size_t size)
    {
        uint32_t state = 0xFFFFFFFF;

        for (size_t i = 0; i < size; i++)
        {
            state = crc32Tab[static_cast<unsigned char>(state) ^ data[i]] ^ (state >> 8);
        }

        return ~state;
    }
}
