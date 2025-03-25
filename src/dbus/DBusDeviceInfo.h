#pragma once

#include "./dbus/DBusDeviceModalias.h"
#include "ObservableVariable.h"

#include <string>
#include <optional>
#include <vector>
#include <sdbus-c++/sdbus-c++.h>

namespace MagicPodsCore {

    class DBusDeviceInfo {
    private:
        std::unique_ptr<sdbus::IProxy> _deviceProxy{};

        std::string _address{};
        DBusDeviceModalias _modalias{};
        std::vector<std::string> _uuids{};
        std::optional<unsigned int> _clazz{};
        std::string _name{};
        ObservableVariable<bool> _connectionStatus{false};

    public:
        explicit DBusDeviceInfo(const sdbus::ObjectPath& objectPath, const std::map<std::string, std::map<std::string, sdbus::Variant>>& interfaces);

        DBusDeviceInfo(const DBusDeviceInfo& info) = delete;
        DBusDeviceInfo(DBusDeviceInfo&& info) noexcept = delete;
        DBusDeviceInfo& operator=(const DBusDeviceInfo& info) = delete;
        DBusDeviceInfo& operator=(DBusDeviceInfo&& info) noexcept = delete;

        const std::string GetAddress() const {
            return _address;
        }

        const DBusDeviceModalias& GetModalias() const {
            return _modalias;
        }

        const std::vector<std::string> GetUuids() const {
            return _uuids;
        }

        const std::optional<unsigned int>& GetClass() const {
            return _clazz;
        }

        const std::string& GetName() const {
            return _name;
        }

        ObservableVariable<bool>& GetConnectionStatus() {
            return _connectionStatus;
        }
    };

}