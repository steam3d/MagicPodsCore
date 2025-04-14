// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    enum class DeviceBatteryStatus : unsigned char
    {
        // Hidden in UI
        NotAvailable = 0x00,
        // Opacity is zero in UI (usable for popup animation)
        Disconnected = 0x01,
        // Show in UI
        Connected = 0x02,
        // Showing battery last updated info even if battery already disconnected.
        Cached = 0x03,
    };

    static std::string DeviceBatteryStatusToString(DeviceBatteryStatus value)
    {
        switch (value)
        {
        case DeviceBatteryStatus::NotAvailable:
            return "NotAvailable";
        case DeviceBatteryStatus::Disconnected:
            return "Disconnected";
        case DeviceBatteryStatus::Connected:
            return "Connected";
        case DeviceBatteryStatus::Cached:
            return "Cached";
        default:
            return "Unknown value";
        }
    }
}