#pragma once

#include "Event.h"
#include "aap/AapClient.h"

namespace MagicPodsCore {

    enum class DeviceBatteryType: unsigned char {
        Single = 0x01, 
        Right = 0x02, 
        Left = 0x04, 
        Case = 0x08, 
    };

    enum class DeviceBatteryStatus: unsigned char {
        // Hidden in UI 
        NotAvailable = 0x00, 
        // Opacity is zero in UI (usable for popup animation)
        Disconnected = 0x01,
        // Show in UI
        Connected = 0x02,
        // Showing battery last updated info even if battery already disconnected. 
        Cached = 0x03, 
    };
    
    struct DeviceBatteryData {
        bool isCharging{};
        DeviceBatteryStatus Status{};
        short Battery{};
    };

    class DeviceBattery {
    private:
        std::map<DeviceBatteryType, DeviceBatteryData> _batteryStatus{};
        Event<std::map<DeviceBatteryType, DeviceBatteryData>> _batteryChanged{};
        bool _cached;
    
    bool UpdateKey(DeviceBatteryType BatteryType, BatteryWatcherData watcherData);

    public:
        Event<std::map<DeviceBatteryType, DeviceBatteryData>>& GetBatteryChangedEvent() {
            return _batteryChanged;
        }

        DeviceBattery(bool cached);
        
        std::map<DeviceBatteryType, DeviceBatteryData> GetBatteryStatus() const {
                return _batteryStatus;
        }

        void UpdateFromAppleBattery(std::map<BatteryType, BatteryWatcherData> appleBattery);
    };
}