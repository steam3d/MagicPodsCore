#pragma once

#include "GalaxyBudsModelIds.h"
#include "GalaxyBudsResponseData.h"
#include "GalaxyBudsMsgTypes.h"
#include "Crc16.h"
#include <vector>
#include <optional>
#include <iostream>

namespace MagicPodsCore{

    class GalaxyBudsPacket{
        private:
        bool isLegacy = false;
        GalaxyBudsModelIds model;

        public:
            unsigned char startOfMessage = 0;
            unsigned char endOfMessage = 0;
            explicit GalaxyBudsPacket(GalaxyBudsModelIds model);
            std::optional<GalaxyBudsResponseData> Extract(const std::vector<unsigned char>& buffer);
            std::vector<unsigned char> Encode(GalaxyBudsMsgIds id, const std::vector<unsigned char>& payload);
    };
}