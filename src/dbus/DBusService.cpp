#include "./dbus/DBusService.h"

namespace MagicPodsCore {

    DBusService::DBusService() : _rootProxy{sdbus::createProxy("org.bluez", "/")},  _defaultBluetoothAdapterProxy{sdbus::createProxy("org.bluez", "/org/bluez/hci0")} {
        //TODO: See comment DeviceFetcher.cpp
        _rootProxy->uponSignal("InterfacesAdded").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces) {
            TryCreateDevice(objectPath, interfaces);
        });
        _rootProxy->uponSignal("InterfacesRemoved").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::vector<std::string> array) {
            TryRemoveDevice(objectPath);
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

    std::vector<std::shared_ptr<DBusDeviceInfo>> DBusService::GetBtDevices() {
        std::vector<std::shared_ptr<DBusDeviceInfo>> devices{};

        std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects{};
        _rootProxy->callMethod("GetManagedObjects").onInterface("org.freedesktop.DBus.ObjectManager").storeResultsTo<std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>>>(managedObjects);

        _cache.clear();

        for (const auto& [objectPath, interfaces] : managedObjects) {
            auto deviceInfo = TryCreateDevice(objectPath, interfaces);
            if (deviceInfo != nullptr) {
                devices.push_back(deviceInfo);
            }
        }
        return devices;
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
                    
                if (_cache.contains(objectPath)) {
                    _cache.erase(objectPath);
                }
                _cache.emplace(objectPath, deviceInfo);

                _onDeviceAddedEvent.FireEvent(deviceInfo);

                return deviceInfo;
            }
        }
        return nullptr;
    }

    bool DBusService::TryRemoveDevice(sdbus::ObjectPath objectPath) {
        if (_cache.contains(objectPath)) {
            _onDeviceRemovedEvent.FireEvent(_cache.at(objectPath));

            _cache.erase(objectPath);

            return true;
        }
        return false;
    }

}