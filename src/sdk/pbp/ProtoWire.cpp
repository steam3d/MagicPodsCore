// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "ProtoWire.h"

namespace MagicPodsCore
{
    void ProtoWire::WriteVarint(std::vector<unsigned char> &writer, uint64_t value)
    {
        while (value > 0x7F)
        {
            writer.push_back(static_cast<unsigned char>((value & 0x7F) | 0x80));
            value >>= 7;
        }
        writer.push_back(static_cast<unsigned char>(value));
    }

    void ProtoWire::WriteVarintField(std::vector<unsigned char> &writer, unsigned int number, uint64_t value)
    {
        WriteVarint(writer, (static_cast<uint64_t>(number) << 3) | static_cast<uint64_t>(ProtoWireTypes::Varint));
        WriteVarint(writer, value);
    }

    void ProtoWire::WriteFixed32Field(std::vector<unsigned char> &writer, unsigned int number, uint32_t value)
    {
        WriteVarint(writer, (static_cast<uint64_t>(number) << 3) | static_cast<uint64_t>(ProtoWireTypes::Fixed32));
        for (int i = 0; i < 4; i++)
        {
            writer.push_back(static_cast<unsigned char>((value >> (i * 8)) & 0xFF));
        }
    }

    void ProtoWire::WriteBytesField(std::vector<unsigned char> &writer, unsigned int number, const std::vector<unsigned char> &value)
    {
        WriteVarint(writer, (static_cast<uint64_t>(number) << 3) | static_cast<uint64_t>(ProtoWireTypes::Bytes));
        WriteVarint(writer, value.size());
        writer.insert(writer.end(), std::begin(value), std::end(value));
    }

    std::optional<std::vector<ProtoField>> ProtoWire::Parse(const std::vector<unsigned char> &data)
    {
        std::vector<ProtoField> fields;
        size_t position = 0;

        auto readVarint = [&data, &position](uint64_t &value) -> bool
        {
            value = 0;
            for (int shift = 0; shift < 64; shift += 7)
            {
                if (position >= data.size())
                    return false;

                unsigned char byte = data[position++];
                value |= static_cast<uint64_t>(byte & 0x7F) << shift;

                if ((byte & 0x80) == 0)
                    return true;
            }
            return false;
        };

        while (position < data.size())
        {
            uint64_t tag = 0;
            if (!readVarint(tag))
                return std::nullopt;

            ProtoField field{};
            field.Number = static_cast<unsigned int>(tag >> 3);
            field.WireType = static_cast<ProtoWireTypes>(tag & 0x07);

            if (field.Number == 0)
                return std::nullopt;

            switch (field.WireType)
            {
            case ProtoWireTypes::Varint:
                if (!readVarint(field.Varint))
                    return std::nullopt;
                break;

            case ProtoWireTypes::Fixed64:
                if (position + 8 > data.size())
                    return std::nullopt;
                for (int i = 0; i < 8; i++)
                {
                    field.Fixed64 |= static_cast<uint64_t>(data[position++]) << (i * 8);
                }
                break;

            case ProtoWireTypes::Bytes:
            {
                uint64_t size = 0;
                if (!readVarint(size))
                    return std::nullopt;
                if (size > data.size() - position)
                    return std::nullopt;

                field.Bytes.assign(std::begin(data) + position, std::begin(data) + position + size);
                position += size;
                break;
            }

            case ProtoWireTypes::Fixed32:
                if (position + 4 > data.size())
                    return std::nullopt;
                for (int i = 0; i < 4; i++)
                {
                    field.Fixed32 |= static_cast<uint32_t>(data[position++]) << (i * 8);
                }
                break;

            default:
                // Groups and unknown wire types are not used by the Maestro protocol.
                return std::nullopt;
            }

            fields.push_back(std::move(field));
        }

        return fields;
    }

    const ProtoField *ProtoWire::Find(const std::vector<ProtoField> &fields, unsigned int number, ProtoWireTypes wireType)
    {
        for (const auto &field : fields)
        {
            if (field.Number == number && field.WireType == wireType)
                return &field;
        }
        return nullptr;
    }
}
