#include "DBusDeviceInfo.h"

#include "Event.h"

#include <memory>
#include <map>
#include <regex>
#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

namespace MagicPodsCore {

    class DBusService {
    private:
        std::unique_ptr<sdbus::IProxy> _rootProxy{};
        std::unique_ptr<sdbus::IProxy> _defaultBluetoothAdapterProxy{};

        std::map<sdbus::ObjectPath, std::shared_ptr<DBusDeviceInfo>> _cache{};

        Event<std::shared_ptr<DBusDeviceInfo>> _onDeviceAddedEvent{};
        Event<std::shared_ptr<DBusDeviceInfo>> _onDeviceRemovedEvent{};

    public:
        explicit DBusService() : _rootProxy{sdbus::createProxy("org.bluez", "/")},  _defaultBluetoothAdapterProxy{sdbus::createProxy("org.bluez", "/org/bluez/hci0")} {
            _rootProxy->uponSignal("InterfacesAdded").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces) {
                TryCreateDevice(objectPath, interfaces);
            });
            _rootProxy->uponSignal("InterfacesRemoved").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::vector<std::string> array) {
                TryRemoveDevice(objectPath);
            });
            _rootProxy->finishRegistration();
        }

        std::shared_ptr<DBusDeviceInfo> TryCreateDevice(sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces) {
            const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]/dev(_[0-9A-F]{2}){6}$"};
            std::smatch match;
            
            if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
                if (interfaces.contains("org.bluez.Device1")) {
                    auto deviceInterface = interfaces.at("org.bluez.Device1");

                    auto deviceInfo = std::make_shared<DBusDeviceInfo>(deviceInterface);
                        
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

        bool TryRemoveDevice(sdbus::ObjectPath objectPath) {
            if (_cache.contains(objectPath)) {
                _onDeviceRemovedEvent.FireEvent(_cache.at(objectPath));

                _cache.erase(objectPath);

                return true;
            }
            return false;
        }

        std::vector<std::shared_ptr<DBusDeviceInfo>> GetBtDevices() {
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
    };

}