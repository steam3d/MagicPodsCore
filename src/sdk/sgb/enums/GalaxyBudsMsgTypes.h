// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore
{

    enum class GalaxyBudsMsgTypes : unsigned char
    {
        Request = 0,
        Response = 1
    };

    static bool isValidGalaxyBudsMsgType(unsigned char value)
    {
        switch (static_cast<GalaxyBudsMsgTypes>(value))
        {
        case GalaxyBudsMsgTypes::Request:
        case GalaxyBudsMsgTypes::Response:
            return true;
        default:
            return false;
        }
    }
}