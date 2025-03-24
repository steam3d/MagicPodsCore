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