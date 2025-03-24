#include "DeviceBattery.h"
#include "device/enums/DeviceBatteryType.h"
#include "device/enums/DeviceBatteryStatus.h"

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
                LOG_RELEASE("%s changed to cache", DeviceBatteryTypeToString(it->Type).c_str());
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
        for (auto &battery : batteryArray)
        {
            bool b = UpdateKey(battery);

            // if any battery is updated we must fire event
            if (b && !isUpdated)
                isUpdated = true;
        }

        if (isUpdated)
        {
            LOG_RELEASE("Battery updated: %s", ToString(_batteryStatus).c_str());
            _batteryChanged.FireEvent(_batteryStatus);
        }
    }

    nlohmann::json DeviceBattery::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        for (const auto &batteryData : GetBatteryStatus())
        {

            auto jsonBatteryData = nlohmann::json::object();
            jsonBatteryData["battery"] = batteryData.Battery;
            jsonBatteryData["charging"] = batteryData.IsCharging;
            jsonBatteryData["status"] = batteryData.Status;

            switch (batteryData.Type)
            {
            case DeviceBatteryType::Single:
                bodyJson["single"] = jsonBatteryData;
                break;

            case DeviceBatteryType::Right:
                bodyJson["right"] = jsonBatteryData;
                break;

            case DeviceBatteryType::Left:
                bodyJson["left"] = jsonBatteryData;
                break;

            case DeviceBatteryType::Case:
                bodyJson["case"] = jsonBatteryData;
                break;
            }
        }
        return bodyJson;
    }

    std::string DeviceBattery::ToString(std::vector<DeviceBatteryData> battery)
    {
        std::string batteryString;
        for (const auto &batteryData : battery)
        {
            switch (batteryData.Type)
            {
            case DeviceBatteryType::Single:
                batteryString += "single:";
                break;

            case DeviceBatteryType::Right:
                batteryString += "right:";
                break;

            case DeviceBatteryType::Left:
                batteryString += "left:";
                break;

            case DeviceBatteryType::Case:
                batteryString += "case:";
                break;
            }

            batteryString+=std::to_string(batteryData.Battery);
            batteryString+="% ";
            batteryString+= batteryData.IsCharging ? "charging " : "";
            batteryString+= DeviceBatteryStatusToString(batteryData.Status);
            batteryString+= " ";
        }
        return batteryString;
    }
}