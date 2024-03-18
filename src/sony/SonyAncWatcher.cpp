#include "SonyAncWatcher.h"
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
                        DummyConvertSonyAncSwitch(State.AncSwitch).c_str(),
                        DummyConvertSonyAncFilter(State.AncFilter).c_str(),
                        DummyConvertSonyAncFilterAmbientVoice(State.AmbientVoice).c_str(),
                        State.Volume);
                        
            _event.FireEvent(State);
            SonyBaseWatcher::ProcessResponse(bytes);
        }
    }

    std::string SonyAncWatcher::DummyConvertSonyAncSwitch(SonyAncSwitch status)
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

    std::string SonyAncWatcher::DummyConvertSonyAncFilter(SonyAncFilter status)
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

    std::string SonyAncWatcher::DummyConvertSonyAncFilterAmbientVoice(SonyAncFilterAmbientVoice status)
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