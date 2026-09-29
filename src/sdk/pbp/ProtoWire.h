// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <vector>
#include <optional>
#include <cstdint>
#include <cstddef>

namespace MagicPodsCore
{
    enum class ProtoWireTypes : unsigned char
    {
        Varint = 0,
        Fixed64 = 1,
        Bytes = 2,
        Fixed32 = 5,
    };

    struct ProtoField
    {
        unsigned int Number{};
        ProtoWireTypes WireType{};
        uint64_t Varint{};
        uint32_t Fixed32{};
        uint64_t Fixed64{};
        std::vector<unsigned char> Bytes{};
    };

    // Minimal encoder and decoder for the protobuf wire format. Only the field types used by the
    // Maestro protocol are supported, unknown fields are kept as they are read from the wire.
    class ProtoWire
    {
    public:
        static void WriteVarint(std::vector<unsigned char> &writer, uint64_t value);
        static void WriteVarintField(std::vector<unsigned char> &writer, unsigned int number, uint64_t value);
        static void WriteFixed32Field(std::vector<unsigned char> &writer, unsigned int number, uint32_t value);
        static void WriteBytesField(std::vector<unsigned char> &writer, unsigned int number, const std::vector<unsigned char> &value);

        // Returns nothing if the message is malformed.
        static std::optional<std::vector<ProtoField>> Parse(const std::vector<unsigned char> &data);
        static const ProtoField *Find(const std::vector<ProtoField> &fields, unsigned int number, ProtoWireTypes wireType);
    };
}
