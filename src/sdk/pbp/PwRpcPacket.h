// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "StringUtils.h"
#include <vector>
#include <optional>
#include <string>
#include <sstream>
#include <cstdint>

namespace MagicPodsCore
{
    // pw.rpc.packet.PacketType, client to server packets are even, server to client packets are odd.
    enum class PwRpcPacketTypes : unsigned char
    {
        Request = 0,
        Response = 1,
        ClientStream = 2,
        ClientError = 4,
        ServerError = 5,
        ServerStream = 7,
        ClientStreamEnd = 8,
    };

    // pw.rpc.packet.RpcPacket. Like every proto3 message the fields holding their default value
    // are not written to the wire.
    struct PwRpcPacket
    {
        PwRpcPacketTypes Type{};
        uint32_t ChannelId{};
        uint32_t ServiceId{};
        uint32_t MethodId{};
        std::vector<unsigned char> Payload{};
        uint32_t Status{};
        uint32_t CallId{};

        std::vector<unsigned char> Encode() const;
        static std::optional<PwRpcPacket> Decode(const std::vector<unsigned char> &data);

        std::string ToString() const
        {
            std::ostringstream os;
            os << "Type: " << static_cast<int>(Type)
               << " Channel: " << ChannelId
               << std::hex << std::showbase
               << " Service: " << ServiceId
               << " Method: " << MethodId
               << std::dec << std::noshowbase
               << " Status: " << Status
               << " Payload: " << StringUtils::BytesToHexString(Payload);
            return os.str();
        }
    };
}
