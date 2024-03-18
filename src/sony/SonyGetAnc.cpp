
#include "SonyGetAnc.h"
#include "SonyAncHelper.h"
#include "Logger.h"

namespace MagicPodsCore
{
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
            state.AncSwitch = bytes[9] == 1 ? SonyAncSwitch::On : SonyAncSwitch::Off;
            state.AncFilter = static_cast<SonyAncFilter>(bytes[11]);
            state.AmbientVoice = static_cast<SonyAncFilterAmbientVoice>(bytes[13]);
            state.Volume = bytes[14];

            LOG_RELEASE("%s %s %s Volume: %d",
                        SonyAncHelper::DummyConvertSonyAncSwitch(state.AncSwitch).c_str(),
                        SonyAncHelper::DummyConvertSonyAncFilter(state.AncFilter).c_str(),
                        SonyAncHelper::DummyConvertSonyAncFilterAmbientVoice(state.AmbientVoice).c_str(),
                        state.Volume);
            SonyBaseCmd::ProcessResponse(bytes);
        }
    }
}