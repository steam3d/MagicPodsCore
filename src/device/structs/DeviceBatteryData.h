#pragma once

#include "device/enums/DeviceBatteryType.h"
#include "device/enums/DeviceBatteryStatus.h"
#include <algorithm>

namespace MagicPodsCore
{
    struct DeviceBatteryData
    {
        DeviceBatteryType Type{};
        bool IsCharging{};
        DeviceBatteryStatus Status{};
        short Battery{};

        DeviceBatteryData(DeviceBatteryType type, DeviceBatteryStatus status, short battery, bool isCharging)
        :Type(type), Status(status), Battery(std::clamp(battery, static_cast<short>(0), static_cast<short>(100))),IsCharging(isCharging) {}
    };

}