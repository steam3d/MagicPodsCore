#pragma once

#include "ObservableVariable.h"

#include <regex>
#include <string>
#include <optional>
#include <vector>
#include <sdbus-c++/sdbus-c++.h>

namespace MagicPodsCore {

    class DBusDeviceInfo {
    private:
        std::unique_ptr<sdbus::IProxy> _deviceProxy{};

        std::string _address{};
        unsigned short _productId{};
        unsigned short _vendorId{};
        std::vector<std::string> _uuids{};
        std::optional<unsigned int> _clazz{};
        std::string _name{};
        ObservableVariable<bool> _connectionStatus{false};
        ObservableVariable<bool> _pairedStatus{false};

    public:
        explicit DBusDeviceInfo(const sdbus::ObjectPath& objectPath, const std::map<std::string, std::map<std::string, sdbus::Variant>>& interfaces);

        DBusDeviceInfo(const DBusDeviceInfo& info) = delete;
        DBusDeviceInfo(DBusDeviceInfo&& info) noexcept = delete;
        DBusDeviceInfo& operator=(const DBusDeviceInfo& info) = delete;
        DBusDeviceInfo& operator=(DBusDeviceInfo&& info) noexcept = delete;

        const std::string& GetAddress() const {
            return _address;
        }

        unsigned short GetProductId() const {
            return _productId;
        }

        unsigned short GetVendorId() const {
            return _vendorId;
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

        ObservableVariable<bool>& GetPairedStatus() {
            return _pairedStatus;
        }

        void Connect();
        void ConnectAsync(std::function<void(const sdbus::Error*)>&& callback);
        void Disconnect();
        void DisconnectAsync(std::function<void(const sdbus::Error*)>&& callback);

        friend auto operator<=>(const DBusDeviceInfo& t1, const DBusDeviceInfo& t2) {
            return t1.GetAddress() <=> t2.GetAddress();
        }

    private:
        static std::array<unsigned short, 2> ParseVidPid(const std::string& modalias);
    };

}