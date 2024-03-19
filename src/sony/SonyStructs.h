#pragma once
#include "SonyEnums.h"

namespace MagicPodsCore
{
    struct SonyAncState {
        SonyAncSwitch AncSwitch{};
        SonyAncFilter AncFilter{};
        SonyAncFilterAmbientVoice AmbientVoice{};
        unsigned char Volume{};        
    };
}
