// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"

#include <span>
#include <vector>

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 04 00 04 00 58 00 01 00 [14 header bytes] [audio frames]
    //
    // 4-5 bytes:
    // 58 00 - Microphone packet
    //
    // 6-7 bytes:
    // 01 00 - Audio packet
    //
    // 22 byte:
    // Start of the first audio frame
    //
    // Each audio frame contains four header bytes, one length byte, and the
    // encoded audio data
    // ---------------------------------------------------------------------------
    class AapMicrophoneWatcher : public AapWatcher
    {
    public:
        using AudioFrames = std::vector<std::vector<unsigned char>>;

    private:
        static constexpr size_t HeaderLength = 22;
        Event<AudioFrames> _event{};

    public:
        AapMicrophoneWatcher();
        static bool IsAudioPacket(std::span<const unsigned char> data);
        void ProcessResponse(const std::vector<unsigned char>& data) override;

        static bool IsAudioPacket(const std::vector<unsigned char>& data)
        {
            return IsAudioPacket(std::span<const unsigned char>{data});
        }

        Event<AudioFrames>& GetEvent()
        {
            return _event;
        }
    };
}
