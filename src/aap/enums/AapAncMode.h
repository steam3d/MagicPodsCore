#pragma once

namespace MagicPodsCore
{
    // Byte 7 indicates the anc mode
    enum class AapAncMode
    {
        Off = 0x01,
        Anc = 0x02,
        Transparency = 0x03,
        Adaptive = 0x04,
    };

    static std::string AapAncModeToString(AapAncMode value)
    {
        switch (value)
        {
        case AapAncMode::Off:
            return "Off";
        case AapAncMode::Transparency:
            return "Transparency";
        case AapAncMode::Adaptive:
            return "Adaptive";
        case AapAncMode::Anc:
            return "WindCancellation";
        }
    }
}