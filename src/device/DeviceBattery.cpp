#include "DeviceBattery.h"

namespace MagicPodsCore
{

    DeviceBattery::DeviceBattery(bool cached) : _cached(cached)
    {
        for (DeviceBatteryType type : {DeviceBatteryType::Single, DeviceBatteryType::Right, DeviceBatteryType::Left, DeviceBatteryType::Case})
        {
            DeviceBatteryData defaultData(type, DeviceBatteryStatus::NotAvailable, 0, false);
            _batteryStatus.push_back(defaultData);
        }
    }

    void DeviceBattery::ClearBattery()
    {
        for (auto &battery : _batteryStatus)
        {
            battery.Battery = 0;
            battery.IsCharging = false;
            battery.Status = DeviceBatteryStatus::NotAvailable;
        }
    }

    bool DeviceBattery::UpdateKey(const DeviceBatteryData &battery)
    {

        auto it = std::find_if(_batteryStatus.begin(), _batteryStatus.end(), [&battery](const DeviceBatteryData &entry)
                               { return entry.Type == battery.Type; });

        if (it == _batteryStatus.end())
            return false;

        // Do not update the battery, use previews (cached) values. If battery is zero then the previous value empty
        if (_cached && it->Battery > 0 && it->Status == DeviceBatteryStatus::Disconnected)
        {
            if (it->Status != DeviceBatteryStatus::Cached)
            {
                // Say UI that battery from cache now
                it->Status = DeviceBatteryStatus::Cached;
                LOG_RELEASE("Changed to cache");
                return true;
            }
            else
            {
                return false;
            }
        }

        bool isUpdated = false;
        if (it->Battery != battery.Battery)
        {
            it->Battery = battery.Battery;
            isUpdated = true;
        }

        if (it->IsCharging != battery.IsCharging)
        {
            it->IsCharging = battery.IsCharging;
            isUpdated = true;
        }

        if (it->Status != battery.Status)
        {
            it->Status = battery.Status;
            isUpdated = true;
        }
        return isUpdated;
    }

    void DeviceBattery::UpdateBattery(const std::vector<DeviceBatteryData> &batteryArray)
    {
        bool isUpdated = false;
        for (auto &battery : _batteryStatus)
        {
            isUpdated = UpdateKey(battery);
        }

        if (isUpdated)
        {
            LOG_RELEASE("Battery updated");
            _batteryChanged.FireEvent(_batteryStatus);
        }
    }
}