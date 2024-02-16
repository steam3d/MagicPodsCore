#pragma once

#include <sdbus-c++/sdbus-c++.h>

#include "Event.h"
#include "aap/AapClient.h"
#include "DeviceBattery.h"

namespace MagicPodsCore {

    class Device {
    private:
        std::unique_ptr<sdbus::IProxy> _deviceProxy{};

        std::string _name{};
        std::string _address{};
        bool _connected{};
        std::string _modalias{};
        DeviceBattery _battery;
        AncMode _ancMode{};

        Event<bool> _onConnectedPropertyChangedEvent{};

        std::unique_ptr<AapClient> _aapClient{};

        mutable std::mutex _propertyMutex{};

    public:
        Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface);
        // TODO: убрать возможность копирования

        std::string GetName() const {
            std::lock_guard lock{_propertyMutex};
            return _name;
        }

        std::string GetAddress() const {
            std::lock_guard lock{_propertyMutex};
            return _address;
        }

        bool GetConnected() const {
            std::lock_guard lock{_propertyMutex};
            return _connected;
        }
        
        std::string GetModalias() const {
            std::lock_guard lock{_propertyMutex};
            return _modalias;
        }

        std::map<DeviceBatteryType, DeviceBatteryData> GetBatteryStatus() const {
            std::lock_guard lock{_propertyMutex};
            return _battery.GetBatteryStatus();
        }

        AncMode GetAncMode() const {
            std::lock_guard lock{_propertyMutex};
            return _ancMode;
        }

        Event<bool>& GetConnectedPropertyChangedEvent() {
            return _onConnectedPropertyChangedEvent;
        }

        void Connect();

        void Disconnect();

        void SetAnc(AncMode mode);

    private:
        void OnBatteryEvent(const std::map<BatteryType, BatteryWatcherData>& data);
        void OnAncEvent(const AncWatcherData& data);
    };

}