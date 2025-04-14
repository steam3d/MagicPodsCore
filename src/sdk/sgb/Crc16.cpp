// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "Crc16.h"
#include <numeric>

namespace MagicPodsCore
{

    uint16_t Crc16::Checksum(const std::vector<unsigned char> &data)
    {
        return std::accumulate(
                   data.begin(),
                   data.end(),
                   uint16_t(0),
                   [](uint16_t current, unsigned char t)
                   {
                       return crc16Tab[((current >> 8) ^ t) & 0xFF] ^ (current << 8);
                   }) &
               0xFFFF;
    }

    std::vector<unsigned char> Crc16::Checksum(GalaxyBudsMsgIds msgId, const std::vector<unsigned char> &payload)
    {
        std::vector<unsigned char> crcData;
        crcData.reserve(1 + payload.size());
        crcData.push_back(static_cast<unsigned char>(msgId));
        crcData.insert(crcData.end(), payload.begin(), payload.end());

        uint16_t crc = Checksum(crcData);
        return {static_cast<unsigned char>(crc & 0xFF), static_cast<unsigned char>(crc >> 8)};
    }
}