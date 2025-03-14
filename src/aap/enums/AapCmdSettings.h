#pragma once

namespace MagicPodsCore
{
    // Byte 6 indicates the setting type
    enum class AapCmdSettings : unsigned char
    {
        Anc = 0x0d,
    };
}