#include "SonyAncHelper.h"
#include "Logger.h"

namespace MagicPodsCore
{
std::string SonyAncHelper::DummyConvertSonyAncSwitch(SonyAncSwitch status)
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

    std::string SonyAncHelper::DummyConvertSonyAncFilter(SonyAncFilter status)
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

    std::string SonyAncHelper::DummyConvertSonyAncFilterAmbientVoice(SonyAncFilterAmbientVoice status)
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