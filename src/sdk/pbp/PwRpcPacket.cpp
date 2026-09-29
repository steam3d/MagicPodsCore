// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PwRpcPacket.h"
#include "ProtoWire.h"

namespace MagicPodsCore
{
    namespace
    {
        constexpr unsigned int TypeField = 1;
        constexpr unsigned int ChannelIdField = 2;
        constexpr unsigned int ServiceIdField = 3;
        constexpr unsigned int MethodIdField = 4;
        constexpr unsigned int PayloadField = 5;
        constexpr unsigned int StatusField = 6;
        constexpr unsigned int CallIdField = 7;
    }

    std::vector<unsigned char> PwRpcPacket::Encode() const
    {
        std::vector<unsigned char> writer;

        if (Type != PwRpcPacketTypes::Request)
            ProtoWire::WriteVarintField(writer, TypeField, static_cast<uint64_t>(Type));
        if (ChannelId != 0)
            ProtoWire::WriteVarintField(writer, ChannelIdField, ChannelId);
        if (ServiceId != 0)
            ProtoWire::WriteFixed32Field(writer, ServiceIdField, ServiceId);
        if (MethodId != 0)
            ProtoWire::WriteFixed32Field(writer, MethodIdField, MethodId);
        if (!Payload.empty())
            ProtoWire::WriteBytesField(writer, PayloadField, Payload);
        if (Status != 0)
            ProtoWire::WriteVarintField(writer, StatusField, Status);
        if (CallId != 0)
            ProtoWire::WriteVarintField(writer, CallIdField, CallId);

        return writer;
    }

    std::optional<PwRpcPacket> PwRpcPacket::Decode(const std::vector<unsigned char> &data)
    {
        std::optional<std::vector<ProtoField>> optionalFields = ProtoWire::Parse(data);
        if (!optionalFields.has_value())
            return std::nullopt;

        PwRpcPacket packet{};

        for (const auto &field : optionalFields.value())
        {
            switch (field.Number)
            {
            case TypeField:
                if (field.WireType == ProtoWireTypes::Varint)
                    packet.Type = static_cast<PwRpcPacketTypes>(field.Varint & 0xFF);
                break;
            case ChannelIdField:
                if (field.WireType == ProtoWireTypes::Varint)
                    packet.ChannelId = static_cast<uint32_t>(field.Varint);
                break;
            case ServiceIdField:
                if (field.WireType == ProtoWireTypes::Fixed32)
                    packet.ServiceId = field.Fixed32;
                break;
            case MethodIdField:
                if (field.WireType == ProtoWireTypes::Fixed32)
                    packet.MethodId = field.Fixed32;
                break;
            case PayloadField:
                if (field.WireType == ProtoWireTypes::Bytes)
                    packet.Payload = field.Bytes;
                break;
            case StatusField:
                if (field.WireType == ProtoWireTypes::Varint)
                    packet.Status = static_cast<uint32_t>(field.Varint);
                break;
            case CallIdField:
                if (field.WireType == ProtoWireTypes::Varint)
                    packet.CallId = static_cast<uint32_t>(field.Varint);
                break;
            default:
                break;
            }
        }

        return packet;
    }
}
