#include "Sony.h"
#include "Logger.h"

namespace MagicPodsCore
{
    SonyBaseCmd::SonyBaseCmd(std::string tag) : _tag{tag}
    {
    }
    void SonyBaseCmd::PrintAsHex()
    {
        LOG_DEBUG("%s ", _tag.c_str());
        std::vector<unsigned char> bytes = Request(1);
        for (int i = 0; i < bytes.size(); i++)
            printf("%02x", bytes[i]);
    }

    std::vector<unsigned char> SonyBaseCmd::Request(unsigned char prefix){
        return SonyBaseCmd::CreateCompletePacket(SonyBaseCmd::CreatePacketBody(prefix));
    }

    std::vector<unsigned char> SonyBaseCmd::CreateCompletePacket(std::vector<unsigned char> packetBody)
    {
        unsigned char crc = 0;

        for (int i = 0; i < packetBody.size(); i++)
            crc += packetBody[i];

        packetBody.insert(packetBody.begin(), startByte);
        packetBody.insert(packetBody.end(), crc);
        packetBody.insert(packetBody.end(), endByte);

        return packetBody;
    }

    void SonyBaseCmd::ProcessResponse(const std::vector<unsigned char>& bytes){
        IsProcessed = true;
        LOG_RELEASE("Request %s processed", _tag.c_str());
    }
}