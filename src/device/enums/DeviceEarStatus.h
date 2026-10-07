// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class DeviceEarStatus : unsigned char
    {
        NotAvailable = 0x00,
        Worn = 0x01, // Both earbuds are in the ears, or over-ear headphones are worn on the head
        PartiallyWorn = 0x02, // // One of the earbuds (left or right) is removed; not applicable to over-ear headphones
        NotWorn = 0x03, // // Both earbuds are removed, or over-ear headphones are removed from the head
    };

    static std::string DeviceEarStatusToString(DeviceEarStatus value)
    {
        switch (value)
        {
        case DeviceEarStatus::NotAvailable:
            return "NotAvailable";
        case DeviceEarStatus::Worn:
            return "Worn";
        case DeviceEarStatus::PartiallyWorn:
            return "PartiallyWorn";
        case DeviceEarStatus::NotWorn:
            return "NotWorn";
        default:
            return "Unknown value";
        }
    }
}