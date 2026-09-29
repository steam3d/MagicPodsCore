// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <vector>
#include <optional>
#include <cstdint>

namespace MagicPodsCore
{
    enum class MaestroPeers : unsigned char
    {
        Unknown = 0,
        Host = 1,
        Case = 2,
        LeftBtCore = 3,
        RightBtCore = 4,
        LeftSensorHub = 5,
        RightSensorHub = 6,
        LeftSpiBridge = 7,
        RightSpiBridge = 8,
        DebugApp = 9,
        MaestroA = 10,
        LeftTahiti = 11,
        RightTahiti = 12,
        MaestroB = 13,
    };

    // The HDLC address encodes the source and the target of a frame, every pair of peers which can talk to each other has a fixed Pigweed RPC channel assigned to it
    class MaestroAddress
    {
    public:
        static uint32_t FromPeers(MaestroPeers source, MaestroPeers target);
        static MaestroPeers GetSource(uint32_t address);
        static MaestroPeers GetTarget(uint32_t address);
        static std::optional<uint32_t> GetChannelId(MaestroPeers local, MaestroPeers remote);
        static std::optional<uint32_t> GetAddressForChannel(uint32_t channelId);
        // The channel the handshake probe is addressed to, the buds answer with their own channel whichever one they are asked on
        static uint32_t GetProbeChannelId();
    };
}
