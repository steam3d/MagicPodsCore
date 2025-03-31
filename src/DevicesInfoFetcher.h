#pragma once

#include "device/Device.h"
#include "Event.h"
#include "./dbus/DBusService.h"

#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <set>
#include <map>
#include <array>
#include <sdbus-c++/sdbus-c++.h>
#include <json.hpp>

namespace MagicPodsCore {

    class DeviceComparator {
    public:
        bool operator()(const std::shared_ptr<Device>& device1, const std::shared_ptr<Device>& device2) const {
            return device1->GetAddress() < device2->GetAddress();
        }
    };

    class DevicesInfoFetcher {
    private:
        DBusService _dbusService{};

        std::map<std::string, std::shared_ptr<Device>> _devicesMap{}; // address -> device
        std::shared_ptr<Device> _activeDevice{};

        Event<std::shared_ptr<Device>> _onActiveDeviceChangedEvent{};
        Event<std::shared_ptr<Device>> _onDeviceAddEvent{};
        Event<std::shared_ptr<Device>> _onDeviceRemoveEvent{};
        Event<bool> _onDefaultAdapterChangeEnabled{};

    public:
        DevicesInfoFetcher();
        // TODO: запретить копирование и перенос

        std::set<std::shared_ptr<Device>, DeviceComparator> GetDevices() const;
        std::shared_ptr<Device> GetDevice(std::string& deviceAddress) const;
        std::shared_ptr<Device> GetActiveDevice() const {
            return _activeDevice;
        }

        void Connect(const std::string& deviceAddress);
        void Disconnect(const std::string& deviceAddress);
        void SetCapabilities(const nlohmann::json& json);

        bool IsBluetoothAdapterPowered() {
            return _dbusService.IsBluetoothAdapterPowered().GetValue();
        }
        void EnableBluetoothAdapter(); // TODO: выпилить?
        void EnableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback);
        void DisableBluetoothAdapter(); // TODO: выпилить?
        void DisableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback);

        Event<std::shared_ptr<Device>>& GetOnActiveDeviceChangedEvent() {
            return _onActiveDeviceChangedEvent;
        }

        Event<std::shared_ptr<Device>>& GetOnDeviceAddEvent() {
            return _onDeviceAddEvent;
        }

        Event<std::shared_ptr<Device>>& GetOnDeviceRemoveEvent() {
            return _onDeviceRemoveEvent;
        }

        Event<bool>& GetOnDefaultAdapterChangeEnabledEvent() {
            return _onDefaultAdapterChangeEnabled;
        }

    private:
        std::shared_ptr<Device> TryCreateDevice(const std::shared_ptr<DBusDeviceInfo>& deviceInfo);

        void ClearAndFillDevicesMap();
        void TrySelectNewActiveDevice();

        public:
            static std::array<unsigned short, 2>ParseVidPid(const std::string& modalias);
    };
}
