#include "DBusDeviceInfo.h"

#include "ObservableVariable.h"

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

        ObservableVariable<bool> _isBluetoothAdapterPowered{false};

    public:
        explicit DBusService();

        std::vector<std::shared_ptr<DBusDeviceInfo>> GetBtDevices();

        ObservableVariable<bool>& IsBluetoothAdapterPowered() {
            return _isBluetoothAdapterPowered;
        }

    private:
        std::shared_ptr<DBusDeviceInfo> TryCreateDevice(sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces);
        bool TryRemoveDevice(sdbus::ObjectPath objectPath);
    };

}