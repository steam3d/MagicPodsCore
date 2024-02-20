#pragma once

#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <set>
#include <map>

#include <sdbus-c++/sdbus-c++.h>
#include <json.hpp>

#include "Device.h"
#include "Event.h"
#include "DeviceAnc.h"

namespace MagicPodsCore {

    class DeviceComparator {
    public:
        bool operator()(const std::shared_ptr<Device>& device1, const std::shared_ptr<Device>& device2) const {
            return device1->GetAddress() < device2->GetAddress();
        }
    };

    class DevicesInfoFetcher {
    private:
        std::unique_ptr<sdbus::IProxy> _rootProxy{};
        std::unique_ptr<sdbus::IProxy> _defaultBluetoothAdapterProxy{};

        std::map<sdbus::ObjectPath, std::shared_ptr<Device>> _devicesMap{};
        std::shared_ptr<Device> _activeDevice{};

        Event<std::shared_ptr<Device>> _onActiveDeviceChangedEvent{};
        Event<std::set<std::shared_ptr<Device>, DeviceComparator>> _onDevicesAddEvent{};
        Event<std::set<std::shared_ptr<Device>, DeviceComparator>> _onDevicesRemoveEvent{};
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
        void SetAnc(const std::string& deviceAddress, DeviceAncMode mode);

        bool IsBluetoothAdapterPowered();
        void EnableBluetoothAdapter(); // TODO: выпилить?
        void EnableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback);
        void DisableBluetoothAdapter(); // TODO: выпилить?
        void DisableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback);

        Event<std::shared_ptr<Device>>& GetOnActiveDeviceChangedEvent() {
            return _onActiveDeviceChangedEvent;
        }

        Event<std::set<std::shared_ptr<Device>, DeviceComparator>>& GetOnDevicesAddEvent() {
            return _onDevicesAddEvent;
        }

        Event<std::set<std::shared_ptr<Device>, DeviceComparator>>& GetOnDevicesRemoveEvent() {
            return _onDevicesRemoveEvent;
        }

        Event<bool>& GetOnDefaultAdapterChangeEnabledEvent() {
            return _onDefaultAdapterChangeEnabled;
        }

    private:
        std::shared_ptr<Device> TryCreateDevice(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface);

        void ClearAndFillDevicesMap();
        void TrySelectNewActiveDevice();
        
        void OnDevicesAdd(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices);
        void OnDevicesRemove(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices);
    };

}
