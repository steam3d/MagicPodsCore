#include "AapSetAnc.h"

namespace MagicPodsCore
{
    AapSetAnc::AapSetAnc(AapAncMode mode) : AapRequest{"AapSetAnc"}, _mode{mode}
    {
    }

    std::vector<unsigned char> AapSetAnc::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Settings), 0x00, static_cast<unsigned char>(CmdSettings::Anc), static_cast<unsigned char>(_mode), 0x00, 0x00, 0x00};
    }

}