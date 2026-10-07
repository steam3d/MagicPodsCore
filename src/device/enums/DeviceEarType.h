// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class DeviceEarType : unsigned char
    {
        Single = 0x01,
        Right = 0x02,
        Left = 0x04,        
    };

    static std::string DeviceEarTypeToString(DeviceEarType value)
    {
        switch (value)
        {
        case DeviceEarType::Single:
            return "Single";
        case DeviceEarType::Right:
            return "Right";
        case DeviceEarType::Left:
            return "Left";
        default:
            return "Unknown value";
        }
    }

}