#pragma once
#include "../enums/GalaxyBudsMsgIds.h"
#include "../enums/GalaxyBudsModelIds.h"
#include "../enums/GalaxyBudsAnc.h"
#include <vector>

namespace MagicPodsCore
{

    class GalaxyBudsSetAnc
    {
    public:
        const std::vector<unsigned char> Payload;
        const GalaxyBudsMsgIds Id = GalaxyBudsMsgIds::NOISE_CONTROLS;
        explicit GalaxyBudsSetAnc(GalaxyBudsAnc mode) : Payload{static_cast<unsigned char>(mode)} {}
    };
}