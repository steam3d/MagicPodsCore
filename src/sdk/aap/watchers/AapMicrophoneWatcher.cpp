// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapMicrophoneWatcher.h"

#include "sdk/aap/enums/AapCmd.h"

#include <cstdint>

namespace MagicPodsCore
{
    namespace
    {
        constexpr uint16_t AudioPacketSubtype = 0x0001;

        uint16_t ReadUInt16(std::span<const unsigned char> data, size_t offset)
        {
            return static_cast<uint16_t>(data[offset]) |
                   static_cast<uint16_t>(data[offset + 1]) << 8;
        }
    }

    AapMicrophoneWatcher::AapMicrophoneWatcher() : AapWatcher{"AapMicrophoneWatcher"}
    {
    }

    bool AapMicrophoneWatcher::IsAudioPacket(std::span<const unsigned char> data)
    {
        return data.size() >= 8 &&
               data[0] == 0x04 &&
               data[2] == 0x04 &&
               ReadUInt16(data, 4) == static_cast<uint16_t>(AapCmd::Microphone) &&
               ReadUInt16(data, 6) == AudioPacketSubtype;
    }

    void AapMicrophoneWatcher::ProcessResponse(const std::vector<unsigned char>& data)
    {
        if (!IsAudioPacket(data) || data.size() < HeaderLength)
            return;

        AudioFrames frames{};
        size_t offset = HeaderLength;
        while (offset + 5 <= data.size())
        {
            const size_t frameLength = data[offset + 4];
            const size_t frameStart = offset + 5;
            const size_t frameEnd = frameStart + frameLength;
            if (frameEnd > data.size())
                break;

            frames.emplace_back(data.begin() + frameStart, data.begin() + frameEnd);
            offset = frameEnd;
        }

        if (!frames.empty())
            _event.FireEvent(frames);
    }
}
