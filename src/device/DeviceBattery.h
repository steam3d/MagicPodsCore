// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "Logger.h"
#include "device/structs/DeviceBatteryData.h"
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
        static std::string ToString(std::vector<DeviceBatteryData> battery);

    private:
        bool UpdateKey(const DeviceBatteryData &battery);
    };
}