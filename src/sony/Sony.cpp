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

    std::vector<unsigned char> SonyBaseCmd::Request(unsigned char prefix)
    {
        return CreateCompletePacket(CreatePacketBody(prefix));
    }

    std::vector<unsigned char> SonyBaseCmd::CreatePacketBody(unsigned char prefix) const
    {
        return std::vector<unsigned char>();
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

    void SonyBaseCmd::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        IsProcessed = true;
        LOG_RELEASE("Request %s processed", _tag.c_str());
    }

    SonyGetAnc::SonyGetAnc() : SonyBaseCmd{"SonyGetAnc"}
    {
    }

    std::vector<unsigned char> SonyGetAnc::CreatePacketBody(unsigned char prefix) const
    {
        return std::vector<unsigned char>{0x0c, prefix, 0x00, 0x00, 0x00, 0x02, 0x66, 0x02};
    }

    void SonyGetAnc::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        if (bytes.size() == 17 && bytes[6] == 8 && bytes[7] == 103)
        {
            SonyAncState state;
            state.AncSwitch = bytes[9] == 1 ? SonyAncSwitch::On : SonyAncSwitch::Off;
            state.AncFilter = static_cast<SonyAncFilter>(bytes[11]);
            state.AmbientVoice = static_cast<SonyAncFilterAmbientVoice>(bytes[13]);
            state.Volume = bytes[14];

            LOG_RELEASE("%s %s %s Volume: %d",
                DummyConvertSonyAncSwitch(state.AncSwitch).c_str(),
                DummyConvertSonyAncFilter(state.AncFilter).c_str(),
                DummyConvertSonyAncFilterAmbientVoice(state.AmbientVoice).c_str(),
                state.Volume
                );
        }
    }

    std::string SonyGetAnc::DummyConvertSonyAncSwitch(SonyAncSwitch status)
    {
        switch (status)
        {
        case SonyAncSwitch::Off:
            return "Off";
        case SonyAncSwitch::On:
            return "On";
        case SonyAncSwitch::OnAndSave:
            return "OnAndSave";
        default:
            return "Unknown";
        }            
    }

    std::string SonyGetAnc::DummyConvertSonyAncFilter(SonyAncFilter status)
    {
        switch (status)
        {
        case SonyAncFilter::Ambient:
            return "Ambient";
        
        case SonyAncFilter::Anc:
            return "Anc";
        case SonyAncFilter::Wind:
            return "Wind";
        default:
            return "Unknown";
        }
    }

    std::string SonyGetAnc::DummyConvertSonyAncFilterAmbientVoice(SonyAncFilterAmbientVoice status)
    {
        switch (status)
        {
        case SonyAncFilterAmbientVoice::Off:
            return "Off";
        case SonyAncFilterAmbientVoice::On:
            return "On";
        default:
            return "Unknown";
        }
    }
}