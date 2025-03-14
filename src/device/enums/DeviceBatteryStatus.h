#pragma once

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

}