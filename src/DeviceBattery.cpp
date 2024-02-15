#include "DeviceBattery.h"

namespace MagicPodsCore {

    DeviceBattery::DeviceBattery(){
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
        for (const auto& [batteryKey, battery] : appleBattery) {
             switch (batteryKey) {
                case BatteryType::Single:
                    DeviceBattery::UpdateKey(DeviceBatteryType::Single, battery);
                    break;

                case BatteryType::Right:
                    DeviceBattery::UpdateKey(DeviceBatteryType::Right, battery);
                    break;

                case BatteryType::Left:
                    DeviceBattery::UpdateKey(DeviceBatteryType::Left, battery);
                    break;

                case BatteryType::Case:
                    DeviceBattery::UpdateKey(DeviceBatteryType::Case, battery);
                    break;
            }
        }
    }

    void DeviceBattery::UpdateKey(DeviceBatteryType BatteryType, BatteryWatcherData watcherData){
        _batteryStatus[BatteryType].Battery = watcherData.Battery;
        _batteryStatus[BatteryType].isCharging = watcherData.Status == ChargingStatus::Charging;
        _batteryStatus[BatteryType].Status = watcherData.Status == ChargingStatus::Disconnected ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected;
    }    
}