// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "StringUtils.h"
#include "sdk/sgb/enums/GalaxyBudsMsgIds.h"
#include "sdk/sgb/enums/GalaxyBudsMsgTypes.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

namespace MagicPodsCore
{

    struct GalaxyBudsResponseData
    {
        GalaxyBudsMsgIds Id;
        GalaxyBudsMsgTypes Type;
        std::vector<unsigned char> Payload;

        GalaxyBudsResponseData(GalaxyBudsMsgIds id, GalaxyBudsMsgTypes type, const std::vector<unsigned char> &payload)
            : Id(id), Type(type), Payload(payload) {}

        std::string ToString() const
        {
            std::ostringstream os;
            os << "Id: " << static_cast<int>(Id) << " Type:" << static_cast<int>(Type) << "Payload: " << StringUtils::BytesToHexString(Payload);
            return os.str();
        }
    };
}