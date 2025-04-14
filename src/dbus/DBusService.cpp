// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "./dbus/DBusService.h"

namespace MagicPodsCore {

    DBusService::DBusService() : _rootProxy{sdbus::createProxy("org.bluez", "/")},  _defaultBluetoothAdapterProxy{sdbus::createProxy("org.bluez", "/org/bluez/hci0")} {
        //TODO: See comment DeviceFetcher.cpp
        _rootProxy->uponSignal("InterfacesAdded").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces) {
            TryCreateDevice(objectPath, interfaces);
        });
        _rootProxy->uponSignal("InterfacesRemoved").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::vector<std::string> array) {
            if (std::find(array.begin(), array.end(), "org.bluez.Device1") != array.end()) {
                TryRemoveDevice(objectPath);
            }
        });
        _rootProxy->finishRegistration();

        _defaultBluetoothAdapterProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            if (values.contains("Powered")) {
                _isBluetoothAdapterPowered.SetValue(values["Powered"].get<bool>());
            }
        });
        _defaultBluetoothAdapterProxy->finishRegistration();

        _isBluetoothAdapterPowered.SetValue(_defaultBluetoothAdapterProxy->getProperty("Powered").onInterface("org.bluez.Adapter1").get<bool>());
    }

    std::set<std::shared_ptr<DBusDeviceInfo>> DBusService::GetBtDevices() {
        std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects{};
        _rootProxy->callMethod("GetManagedObjects").onInterface("org.freedesktop.DBus.ObjectManager").storeResultsTo<std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>>>(managedObjects);

        _knownDevices.clear();

        for (const auto& [objectPath, interfaces] : managedObjects) {
            TryCreateDevice(objectPath, interfaces);
        }

        return _pairedDevices;
    }

    void DBusService::EnableBluetoothAdapter() {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(true).uponReplyInvoke([this](const sdbus::Error* err) {});
    }

    void DBusService::EnableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(true).uponReplyInvoke(callback);
    }

    void DBusService::DisableBluetoothAdapter() {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(false).uponReplyInvoke([this](const sdbus::Error* err) {});
    }

    void DBusService::DisableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(false).uponReplyInvoke(callback);
    }

    std::shared_ptr<DBusDeviceInfo> DBusService::TryCreateDevice(sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces) {
        const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]/dev(_[0-9A-F]{2}){6}$"};
        std::smatch match;
        
        if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
            if (interfaces.contains("org.bluez.Device1")) {
                auto deviceInfo = std::make_shared<DBusDeviceInfo>(objectPath, interfaces);
                    
                if (_knownDevices.contains(objectPath)) {
                    _knownDevices.erase(objectPath);
                }
                _knownDevices.emplace(objectPath, deviceInfo);

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

    bool DBusService::TryRemoveDevice(sdbus::ObjectPath objectPath) {
        if (_knownDevices.contains(objectPath)) {
            const auto& deviceInfo = _knownDevices.at(objectPath);

            _onDeviceRemovedEvent.FireEvent(deviceInfo);

            if (_pairedDevices.contains(deviceInfo)) {
                _pairedDevices.erase(deviceInfo);
            }
            _knownDevices.erase(objectPath);

            return true;
        }
        return false;
    }

}