#include "AapAncWatcher.h"

namespace MagicPodsCore
{
    AapAncWatcher::AapAncWatcher() : AapWatcher{"AapAncWatcher"}
    {
    }

    void AapAncWatcher::ProcessBytes(const std::vector<unsigned char> &bytes)
    {
        // min length of the anc packet
        if (bytes.size() < 10)
            return;

        // packet type must be Settings
        if (bytes[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        // settings type must be Anc
        if (bytes[6] != static_cast<unsigned char>(AapCmdSettings::Anc))
            return;

        LOG_DEBUG("%s", _tag.c_str());

        AapAncMode ancMode = static_cast<AapAncMode>(bytes[7]);

        // REPLACE WITH ANC STORAGE LOGIC OR EVENT ONANCCHANGED?
        LOG_DEBUG("%s", DummyConvertAncMode(ancMode).c_str());
        _event.FireEvent(AncWatcherData{_tag, ancMode});
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