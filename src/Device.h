#pragma once

#include <sdbus-c++/sdbus-c++.h>

#include "Event.h"
#include "aap/AapClient.h"

namespace MagicPodsCore {

    class Device {
    private:
        std::unique_ptr<sdbus::IProxy> _deviceProxy{};

        std::string _name{};
        std::string _address{};
        bool _connected{};
        std::string _modalias{};
        std::map<BatteryType, BatteryWatcherData> _batteryStatus{};
        AncMode _ancMode{};

        Event<bool> _onConnectedPropertyChangedEvent{};

        std::unique_ptr<AapClient> _aapClient{};

        mutable std::mutex _propertyMutex{};

    public:
        static std::shared_ptr<Device> TryCreateDevice(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface);

        Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface);
        // TODO: убрать возможность копирования

        std::string GetName() const {
            std::lock_guard{_propertyMutex};
            return _name;
        }

        std::string GetAddress() const {
            std::lock_guard{_propertyMutex};
            return _address;
        }

        bool GetConnected() const {
            std::lock_guard{_propertyMutex};
            return _connected;
        }
        
        std::string GetModalias() const {
            std::lock_guard{_propertyMutex};
            return _modalias;
        }

        std::map<BatteryType, BatteryWatcherData> GetBatteryStatus() const {
            std::lock_guard{_propertyMutex};
            return _batteryStatus;
        }

        AncMode GetAncMode() const {
            std::lock_guard{_propertyMutex};
            return _ancMode;
        }

        Event<bool>& GetConnectedPropertyChangedEvent() {
            return _onConnectedPropertyChangedEvent;
        }

        void Connect();

        void Disconnect();

        void SetAnc(AncMode mode);

    private:
        void OnBatteryEvent(const BatteryWatcherData& data);
        void OnAncEvent(const AncWatcherData& data);
    };

}