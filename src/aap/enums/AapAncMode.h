#pragma once

namespace MagicPodsCore
{
    // Byte 7 indicates the anc mode
    enum class AapAncMode
    {
        Off = 0x01,
        On = 0x02,
        Tra = 0x03
    };

}