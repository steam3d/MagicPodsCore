#pragma once

#include "../Event.h"
#include "../Logger.h"
#include "enums/DeviceBatteryStatus.h"
#include "enums/DeviceBatteryType.h"
#include "structs/DeviceBatteryData.h"
#include <json.hpp>

namespace MagicPodsCore
{
    class DeviceBattery
    {
    private:
        std::vector<DeviceBatteryData> _batteryStatus{};
        Event<std::vector<DeviceBatteryData>> _batteryChanged{};
        bool _cached{};

    public:
        explicit DeviceBattery(bool cached);

        Event<std::vector<DeviceBatteryData>> &GetBatteryChangedEvent()
        {
            return _batteryChanged;
        }

        std::vector<DeviceBatteryData> GetBatteryStatus() const
        {
            return _batteryStatus;
        }

        void UpdateBattery(const std::vector<DeviceBatteryData> &batteryArray);
        void ClearBattery();
        nlohmann::json CreateJsonBody();

    private:
        bool UpdateKey(const DeviceBatteryData &battery);
    };
}