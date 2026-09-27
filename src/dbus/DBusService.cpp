// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "./dbus/DBusService.h"
#include "Logger.h"

#include <algorithm>
#include <regex>
#include <utility>

namespace MagicPodsCore {

    namespace {
        constexpr auto BLUEZ_ADAPTER_INTERFACE = "org.bluez.Adapter1";
        constexpr auto BLUEZ_DEVICE_INTERFACE = "org.bluez.Device1";
        constexpr auto DBUS_OBJECT_MANAGER_INTERFACE = "org.freedesktop.DBus.ObjectManager";
    }

    DBusService::DBusService() : _rootProxy{sdbus::createProxy("org.bluez", "/")} {
        //TODO: See comment DeviceFetcher.cpp
        _rootProxy->uponSignal("InterfacesAdded").onInterface(DBUS_OBJECT_MANAGER_INTERFACE).call([this](sdbus::ObjectPath objectPath, InterfaceMap interfaces) {
            HandleInterfacesAdded(objectPath, interfaces);
        });
        _rootProxy->uponSignal("InterfacesRemoved").onInterface(DBUS_OBJECT_MANAGER_INTERFACE).call([this](sdbus::ObjectPath objectPath, std::vector<std::string> interfaces) {
            HandleInterfacesRemoved(objectPath, interfaces);
        });
        _rootProxy->finishRegistration();

        FetchDevices();
    }

    DBusService::~DBusService() {
        // Stop ObjectManager callbacks before the objects used by them are destroyed.
        _rootProxy.reset();
    }

    void DBusService::HandleInterfacesAdded(const sdbus::ObjectPath& objectPath, const InterfaceMap& interfaces) {
        try {
            if (interfaces.contains(BLUEZ_ADAPTER_INTERFACE)) {
                _bluetoothAdapter.AddAdapter(objectPath);
            }
            TryCreateDevice(objectPath, interfaces);
            TryUpdateInterfaceAddedForDevice(objectPath, interfaces);
        }
        catch (const std::exception& error) {
            Logger::Error("Failed to handle BlueZ InterfacesAdded for %s: %s", objectPath.c_str(), error.what());
        }
    }

    void DBusService::HandleInterfacesRemoved(const sdbus::ObjectPath& objectPath, const std::vector<std::string>& interfaces) {
        try {
            if (std::find(interfaces.begin(), interfaces.end(), BLUEZ_DEVICE_INTERFACE) != interfaces.end()) {
                TryRemoveDevice(objectPath);
            }
            if (std::find(interfaces.begin(), interfaces.end(), BLUEZ_ADAPTER_INTERFACE) != interfaces.end()) {
                _bluetoothAdapter.RemoveAdapter(objectPath);
            }
        }
        catch (const std::exception& error) {
            Logger::Error("Failed to handle BlueZ InterfacesRemoved for %s: %s", objectPath.c_str(), error.what());
        }
    }

    std::set<std::shared_ptr<DBusDeviceInfo>> DBusService::GetAllDevices() {
        std::set<std::shared_ptr<DBusDeviceInfo>> devices;
        for (const auto& [path, device] : _knownDevices) {
            devices.emplace(device);
        }
        return devices;
    }

    std::set<std::shared_ptr<DBusDeviceInfo>> DBusService::GetPairedDevices() {
        return _pairedDevices;
    }

    void DBusService::EnableBluetoothAdapter() {
        _bluetoothAdapter.SetPowered(true);
    }

    void DBusService::EnableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _bluetoothAdapter.SetPoweredAsync(true, std::move(callback));
    }

    void DBusService::DisableBluetoothAdapter() {
        _bluetoothAdapter.SetPowered(false);
    }

    void DBusService::DisableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _bluetoothAdapter.SetPoweredAsync(false, std::move(callback));
    }

    void DBusService::SetDiscoveryFilter(const std::map<std::string, sdbus::Variant> &filter) {
        _bluetoothAdapter.SetDiscoveryFilter(filter);
    }

    void DBusService::SetDiscoveryFilterAsync(const std::map<std::string, sdbus::Variant> &filter, std::function<void(const sdbus::Error *)> &&callback) {
        _bluetoothAdapter.SetDiscoveryFilterAsync(filter, std::move(callback));
    }

    void DBusService::StartDiscovery() {
        _bluetoothAdapter.StartDiscovery();
    }

    void DBusService::StartDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _bluetoothAdapter.StartDiscoveryAsync(std::move(callback));
    }

    void DBusService::StopDiscovery() {
        _bluetoothAdapter.StopDiscovery();
    }

    void DBusService::StopDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _bluetoothAdapter.StopDiscoveryAsync(std::move(callback));
    }

    void DBusService::FetchDevices() {
        std::map<sdbus::ObjectPath, InterfaceMap> managedObjects{};
        try {
            _rootProxy->callMethod("GetManagedObjects").onInterface(DBUS_OBJECT_MANAGER_INTERFACE).storeResultsTo<std::map<sdbus::ObjectPath, InterfaceMap>>(managedObjects);
        }
        catch (const sdbus::Error& error) {
            Logger::Error("Failed to enumerate BlueZ objects: %s", error.what());
            return;
        }

        _knownDevices.clear();
        _pairedDevices.clear();

        // Discover adapters first so device operations immediately have a target.
        for (const auto& [objectPath, interfaces] : managedObjects) {
            if (interfaces.contains(BLUEZ_ADAPTER_INTERFACE)) {
                _bluetoothAdapter.AddAdapter(objectPath);
            }
        }

        for (const auto& [objectPath, interfaces] : managedObjects) {
            TryCreateDevice(objectPath, interfaces);
        }
    }

    std::shared_ptr<DBusDeviceInfo> DBusService::TryCreateDevice(
        const sdbus::ObjectPath& objectPath,
        const InterfaceMap& interfaces) {
        const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]+/dev(_[0-9A-F]{2}){6}$"};
        std::smatch match;
        
        if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
            if (interfaces.contains("org.bluez.Device1")) {
                auto deviceInfo = std::make_shared<DBusDeviceInfo>(objectPath, interfaces);
                    
                if (_knownDevices.contains(objectPath)) {
                    _knownDevices.erase(objectPath);
                }
                _knownDevices.emplace(objectPath, deviceInfo);
                _onAnyDeviceAddedEvent.FireEvent(deviceInfo);

                if (deviceInfo->GetPairedStatus().GetValue()) {
                    _pairedDevices.emplace(deviceInfo);
                    _onDeviceAddedEvent.FireEvent(deviceInfo);
                }
                else {
                    deviceInfo->GetPairedStatus().GetEvent().Subscribe([this, objectPath](size_t listenerId, bool newValue) {
                        if (newValue && _knownDevices.contains(objectPath)) {
                            auto device = _knownDevices.at(objectPath);
                            if (!_pairedDevices.contains(device)) {
                                _pairedDevices.emplace(device);
                                _onDeviceAddedEvent.FireEvent(device);
                            }
                        }
                    });
                }

                return deviceInfo;
            }
        }
        return nullptr;
    }

    bool DBusService::TryRemoveDevice(const sdbus::ObjectPath& objectPath) {
        if (_knownDevices.contains(objectPath)) {
            const auto deviceInfo = _knownDevices.at(objectPath);

            // Device removal tears down subscribers which may otherwise race a
            // PropertiesChanged callback. Join the proxy event loop first.
            deviceInfo->StopListening();

            _onDeviceRemovedEvent.FireEvent(deviceInfo);

            if (_pairedDevices.contains(deviceInfo)) {
                _pairedDevices.erase(deviceInfo);
            }
            _knownDevices.erase(objectPath);

            return true;
        }
        return false;
    }

    bool DBusService::TryUpdateInterfaceAddedForDevice(
        const sdbus::ObjectPath& objectPath,
        const InterfaceMap& interfaces)
    {
        const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]+/dev(_[0-9A-F]{2}){6}$"};
        std::smatch match;
        
        if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
            if (_knownDevices.contains(objectPath)) {
                auto device = _knownDevices.at(objectPath);
                device->InterfaceAdded(interfaces);
                return true;
            }
        }
        return false;
    }
    
    bool DBusService::TryUpdateInterfaceRemovedForDevice(const sdbus::ObjectPath& objectPath)
    {
        return false;
    }
}
