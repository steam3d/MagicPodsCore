#pragma once

namespace MagicPodsCore
{
    enum class DeviceBatteryType : unsigned char
    {
        Single = 0x01,
        Right = 0x02,
        Left = 0x04,
        Case = 0x08,
    };

}