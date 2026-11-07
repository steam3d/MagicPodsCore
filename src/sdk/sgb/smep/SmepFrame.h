// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// SMEP ("alternative") frame codec

#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace MagicPodsCore
{
    // One SMEP frame, as seen on RFCOMM scn 29:
    //   0xFC | size lo | (size hi | flags) | id | payload... | crc lo | crc hi | 0xCC
    // size = 1 (id) + payload + 2 (crc). Flags live in the top six bits of the size word
    // (the buds push with 0x5000, we send with 0xC000). The checksum is the same CRC-16
    // (poly 0x1021, init 0) the main channel uses, stored little-endian, so a valid frame
    // has residue 0 over id + payload + crc_hi + crc_lo.
    //
    // SMEP id 0x01 (PROFILE_CONTROL) carries a sub-message in payload[0]:
    //   0x43 write MDE version      0x44 read attributes (44 00 02 = the device group table)
    //   0x45 push attributes/state  0x71 focus claim / own device class (71 02, 71 82)
    struct SmepFrame
    {
        static constexpr uint8_t SOM = 0xFC;
        static constexpr uint8_t EOM = 0xCC;
        // Our own frames: the class/focus sub-messages go out with these flags.
        static constexpr uint16_t REQUEST_FLAGS = 0xC000;

        uint8_t id = 0;
        uint16_t flags = 0;
        std::vector<uint8_t> payload;

        static std::vector<uint8_t> Encode(uint8_t id, const std::vector<uint8_t>& payload, uint16_t flags = REQUEST_FLAGS);

        // Pulls every complete frame out of `buffer` and removes the bytes it consumed.
        // Bytes that cannot be a frame header are skipped, which is what makes a partial
        // read or a stray byte recoverable instead of killing the channel.
        static std::vector<SmepFrame> Decode(std::vector<uint8_t>& buffer);
    };
}
