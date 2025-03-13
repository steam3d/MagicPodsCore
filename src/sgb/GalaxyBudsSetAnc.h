#pragma once
#include "GalaxyBudsMsgIds.h"
#include "GalaxyBudsModelIds.h"
#include "GalaxyBudsAnc.h"
#include <vector>

namespace MagicPodsCore{
    class GalaxyBudsSetAnc
    {
        public:
            const std::vector<unsigned char>Payload;
            const GalaxyBudsMsgIds Id = GalaxyBudsMsgIds::NOISE_CONTROLS;
            explicit GalaxyBudsSetAnc(GalaxyBudsAnc mode): Payload{static_cast<unsigned char>(mode)} {}
    };
}