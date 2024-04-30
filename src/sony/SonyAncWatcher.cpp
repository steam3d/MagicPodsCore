#include "SonyAncWatcher.h"
#include "SonyAncHelper.h"
#include "Logger.h"

namespace MagicPodsCore
{
    SonyAncWatcher::SonyAncWatcher() : SonyBaseWatcher{"SonyAncWatcher"}
    {
    }

    void SonyAncWatcher::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        if (bytes.size() == 17 && bytes[6] == 8 && bytes[7] == 105)
        {            
            State.AncSwitch = bytes[9] == 1 ? SonyAncSwitch::On : SonyAncSwitch::Off;
            State.AncFilter = static_cast<SonyAncFilter>(bytes[11]);
            State.AmbientVoice = static_cast<SonyAncFilterAmbientVoice>(bytes[13]);
            State.Volume = bytes[14];
        
            LOG_RELEASE("%s %s %s Volume: %d",
                        SonyAncHelper::DummyConvertSonyAncSwitch(State.AncSwitch).c_str(),
                        SonyAncHelper::DummyConvertSonyAncFilter(State.AncFilter).c_str(),
                        SonyAncHelper::DummyConvertSonyAncFilterAmbientVoice(State.AmbientVoice).c_str(),
                        State.Volume);

            _event.FireEvent(State);
        }
    }
}