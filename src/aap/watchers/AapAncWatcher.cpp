#include "AapAncWatcher.h"

namespace MagicPodsCore
{
    AapAncWatcher::AapAncWatcher() : AapWatcher{"AapAncWatcher"}
    {
    }

    void AapAncWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        // min length of the anc packet
        if (data.size() < 10)
            return;

        // packet type must be Settings
        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        // settings type must be Anc
        if (data[6] != static_cast<unsigned char>(AapCmdSettings::Anc))
            return;

        AapAncMode ancMode = static_cast<AapAncMode>(data[7]);

        LOG_DEBUG("%s: %s", _tag.c_str(), DummyConvertAncMode(ancMode).c_str());
        _event.FireEvent(ancMode);
    }

    std::string AapAncWatcher::DummyConvertAncMode(AapAncMode mode)
    {
        switch (mode)
        {
        case AapAncMode::Anc:
            return "On";
        case AapAncMode::Off:
            return "Off";
        case AapAncMode::Transparency:
            return "Tra";
        default:
            return "Unknown";
        }
    }

}