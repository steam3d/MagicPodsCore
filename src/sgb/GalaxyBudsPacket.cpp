#include "GalaxyBudsPacket.h"

namespace MagicPodsCore
{

    GalaxyBudsPacket::GalaxyBudsPacket(GalaxyBudsModelIds model) : model(model)
    {
        isLegacy = model == GalaxyBudsModelIds::GalaxyBuds;
    }

    std::optional<GalaxyBudsResponseData> GalaxyBudsPacket::Extract(const std::vector<unsigned char> &buffer)
    {
        if (buffer.size() < 6)
        {
            return std::nullopt;
        }

        startOfMessage = buffer[0];
        endOfMessage = buffer[buffer.size() - 1];

        GalaxyBudsMsgTypes type;
        uint16_t size;

        if (isLegacy)
        {
            // CHECK THIS
            unsigned char typeChar = buffer[1];
            if (!isValidGalaxyBudsMsgType(typeChar))
            {
                return std::nullopt;
            }

            type = static_cast<GalaxyBudsMsgTypes>(buffer[1]);
            size = buffer[2];
        }
        else
        {
            unsigned short header = (static_cast<unsigned short>(buffer[2]) << 8) | buffer[1]; // to big Endian
            bool isFragment = (header & 0x2000) != 0;
            type = (header & 0x1000) != 0 ? GalaxyBudsMsgTypes::Request : GalaxyBudsMsgTypes::Response;
            size = header & 0x3FF;
        }

        unsigned char idChar = buffer[3];
        if (!isValidGalaxyBudsMsgIdsType(idChar))
        {
            return std::nullopt;
        }
        GalaxyBudsMsgIds id = static_cast<GalaxyBudsMsgIds>(idChar);

        // packet size -id - CRC1 - CRC2
        uint16_t payloadSize = size - 3;
        if (payloadSize < 0)
        {
            payloadSize = 0;
            size = 3;
        }

        std::vector<unsigned char> crcData;
        crcData.reserve(size);
        crcData.push_back(static_cast<unsigned char>(id)); //[0]
        for (uint16_t i = 0; i < payloadSize; i++)
        {
            crcData.push_back(buffer[4 + i]);
        }

        // Swap crc1 and crc2
        crcData.push_back(buffer[3 + payloadSize + 2]);
        crcData.push_back(buffer[3 + payloadSize + 1]);

        uint16_t crc16 = Crc16::Checksum(crcData);
        if (crc16 != 0)
        {
            std::cout << "Invalid checksum" << std::endl;
            return std::nullopt;
        }

        std::vector<unsigned char> payload;
        payload.reserve(payloadSize);
        for (uint16_t i = 0; i < payloadSize; i++)
        {
            payload.push_back(buffer[4 + i]);
        }

        return GalaxyBudsResponseData(id, type, payload);
    }

    std::vector<unsigned char> GalaxyBudsPacket::Encode(GalaxyBudsMsgIds id, const std::vector<unsigned char> &payload)
    {
        const uint8_t CrcSize = 2;
        const uint8_t MsgIdSize = 1;
        const uint8_t SomSize = 1;
        const uint8_t EomSize = 1;
        const uint8_t TypeSize = 1;
        const uint8_t BytesSize = 1;

        GalaxyBudsMsgTypes type = GalaxyBudsMsgTypes::Request;
        uint16_t size = MsgIdSize + payload.size() + CrcSize;
        uint16_t totalPacketSize = SomSize + TypeSize + BytesSize + MsgIdSize + payload.size() + CrcSize + EomSize;

        std::vector<unsigned char> writer;
        writer.reserve(totalPacketSize);

        writer.push_back(startOfMessage);

        if (isLegacy)
        {
            writer.push_back(static_cast<unsigned char>(type));
            writer.push_back(static_cast<unsigned char>(size)); // DANGER
        }
        else
        {
            // to little endian
            writer.push_back(static_cast<unsigned char>(size & 0xFF));
            writer.push_back(static_cast<unsigned char>(size >> 8));

            // if (IsFragment)
            //{
            //     header[1] = (byte)(header[1] | 32);
            // }
            // if (Type == MsgTypes.Response)
            //{
            //     header[1] = (byte)(header[1] | 16);
            // }
        }

        writer.push_back(static_cast<unsigned char>(id));
        writer.insert(writer.end(), std::begin(payload), std::end(payload));

        std::vector<unsigned char> crc = Crc16::Checksum(id, payload);
        writer.insert(writer.end(), std::begin(crc), std::end(crc));
        writer.push_back(endOfMessage);

        return writer;
    }
}