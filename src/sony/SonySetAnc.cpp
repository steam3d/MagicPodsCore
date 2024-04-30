
#include "SonySetAnc.h"
#include "Logger.h"

namespace MagicPodsCore
{
    SonySetAnc::SonySetAnc(SonyAncState state) : SonyBaseCmd{"SonySetAnc"}
    {
        SonySetAnc::state = state;
    }

    std::vector<unsigned char> SonySetAnc::CreatePacketBody(unsigned char prefix) const
    {
        return std::vector<unsigned char>{0x0c, prefix, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, (unsigned char)state.AncSwitch, 0x02, (unsigned char)state.AncFilter, 0x01, (unsigned char)state.AmbientVoice, state.Volume };
    }

    void SonySetAnc::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        if (bytes.size() == 9 && bytes[1] == 1)
        {            
            SonyBaseCmd::ProcessResponse(bytes);
        }
    }
}