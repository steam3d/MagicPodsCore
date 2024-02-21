#include "DeviceBattery.h"

#include "Logger.h"

namespace MagicPodsCore {

    DeviceBattery::DeviceBattery(bool cached): _cached(cached) {
        ClearBattery();
    }

    void DeviceBattery::ClearBattery(){
        struct DeviceBatteryData defaultData;
        defaultData.Battery = 0;
        defaultData.isCharging = false;
        defaultData.Status = DeviceBatteryStatus::NotAvailable;
        
        _batteryStatus[DeviceBatteryType::Single] = defaultData;         
        _batteryStatus[DeviceBatteryType::Left] = defaultData;         
        _batteryStatus[DeviceBatteryType::Right] = defaultData;         
        _batteryStatus[DeviceBatteryType::Case] = defaultData;         
    }

    void DeviceBattery::UpdateFromAppleBattery(std::map<BatteryType, BatteryWatcherData> appleBattery) {
        bool isUpdated = false;
        
        for (const auto& [batteryKey, battery] : appleBattery) {
            bool isUpdatedKey = false;
             switch (batteryKey) {
                case BatteryType::Single:
                    isUpdatedKey = DeviceBattery::UpdateKey(DeviceBatteryType::Single, battery);
                    break;
                case BatteryType::Right:
                    isUpdatedKey = DeviceBattery::UpdateKey(DeviceBatteryType::Right, battery);
                    break;
                case BatteryType::Left:
                    isUpdatedKey = DeviceBattery::UpdateKey(DeviceBatteryType::Left, battery);
                    break;
                case BatteryType::Case:
                    isUpdatedKey = DeviceBattery::UpdateKey(DeviceBatteryType::Case, battery);
                    break;
                }
                // Any update battery must trigger event
                if (isUpdatedKey == true) {
                    isUpdated = true;
                }
        }

        if (isUpdated) {
            LOG_RELEASE("Battery updated");
            _batteryChanged.FireEvent(_batteryStatus);
        }
    }

    bool DeviceBattery::UpdateKey(DeviceBatteryType BatteryType, BatteryWatcherData watcherData){
        auto battery = watcherData.Battery;
        auto isCharging = watcherData.Status == ChargingStatus::Charging;
        auto status = watcherData.Status == ChargingStatus::Disconnected ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected;

        
        // Do not update the battery, use previews (cached) values. If battery is zero then the previous value empty 
        if (_cached && _batteryStatus[BatteryType].Battery > 0 && status == DeviceBatteryStatus::Disconnected){
            if (_batteryStatus[BatteryType].Status != DeviceBatteryStatus::Cached){
                // Say UI that battery from cache now
                _batteryStatus[BatteryType].Status = DeviceBatteryStatus::Cached;
                LOG_RELEASE("Changed to cache");
                return true;
            }
            else{
                
                return false;
            }            
        }
        
        bool isUpdated = false;
        if (_batteryStatus[BatteryType].Battery != battery){
             _batteryStatus[BatteryType].Battery = battery;
            isUpdated = true;
        }

        if (_batteryStatus[BatteryType].isCharging != isCharging){
            _batteryStatus[BatteryType].isCharging = isCharging;
            isUpdated = true;
        }

        if (_batteryStatus[BatteryType].Status != status){
            _batteryStatus[BatteryType].Status = status;
            isUpdated = true;
        }
        return isUpdated;
    }    
}