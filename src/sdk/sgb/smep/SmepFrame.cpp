// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// SMEP frame codec

#include "SmepFrame.h"
#include "sdk/sgb/Crc16.h"

#include <cstring>

namespace MagicPodsCore
{
    std::vector<uint8_t> SmepFrame::Encode(uint8_t id, const std::vector<uint8_t>& payload, uint16_t flags)
    {
        const uint16_t size = static_cast<uint16_t>(1 + payload.size() + 2);
        std::vector<uint8_t> body;
        body.reserve(1 + payload.size());
        body.push_back(id);
        body.insert(body.end(), payload.begin(), payload.end());

        const auto crc = Crc16::Checksum(static_cast<GalaxyBudsMsgIds>(id), payload);

        std::vector<uint8_t> out;
        out.reserve(body.size() + 6);
        out.push_back(SOM);
        out.push_back(static_cast<uint8_t>(size & 0xFF));
        out.push_back(static_cast<uint8_t>((size >> 8) | (flags >> 8)));
        out.insert(out.end(), body.begin(), body.end());
        out.push_back(crc[0]);
        out.push_back(crc[1]);
        out.push_back(EOM);
        return out;
    }

    std::vector<SmepFrame> SmepFrame::Decode(std::vector<uint8_t>& buffer)
    {
        std::vector<SmepFrame> frames;

        size_t i = 0;
        const size_t n = buffer.size();
        size_t consumed = 0;
        while (i + 3 <= n)
        {
            if (buffer[i] != SOM)
            {
                ++i;
                continue;
            }

            const uint16_t raw = static_cast<uint16_t>(buffer[i + 1] | (buffer[i + 2] << 8));
            const uint16_t flags = raw & 0xFC00;
            const uint16_t size = raw & 0x03FF;
            const size_t total = 3 + size + 1;   // SOM + size word + (id + payload + crc) + EOM
            if (size < 3 || i + total > n || buffer[i + total - 1] != EOM)
            {
                ++i;
                continue;
            }

            const uint8_t id = buffer[i + 3];
            const auto begin = buffer.begin() + static_cast<ptrdiff_t>(i + 4);
            std::vector<uint8_t> payload(begin, begin + (size - 3));

            // Residue check: CRC over id + payload + crc_hi + crc_lo has to be zero.
            std::vector<uint8_t> crcData;
            crcData.reserve(size);
            crcData.push_back(id);
            crcData.insert(crcData.end(), payload.begin(), payload.end());
            crcData.push_back(buffer[i + size + 2]);
            crcData.push_back(buffer[i + size + 1]);
            if (Crc16::Checksum(crcData) != 0)
            {
                ++i;
                continue;
            }

            frames.push_back(SmepFrame{id, flags, std::move(payload)});
            i += total;
            consumed = i;
        }

        // Keep only what could not be parsed yet.
        if (consumed > 0)
        {
            buffer.erase(buffer.begin(), buffer.begin() + static_cast<ptrdiff_t>(consumed));
        }
        return frames;
    }
}
