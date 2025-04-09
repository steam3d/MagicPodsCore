#include "./dbus/DBusDeviceInfo.h"

#include "Logger.h"

namespace MagicPodsCore {

    DBusDeviceInfo::DBusDeviceInfo(const sdbus::ObjectPath& objectPath, const std::map<std::string, std::map<std::string, sdbus::Variant>>& interfaces) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)}
    {
        auto deviceInterface = interfaces.at("org.bluez.Device1");

        if (deviceInterface.contains("Address")) {
            _address = deviceInterface.at("Address").get<std::string>();
        }

        if (deviceInterface.contains("Modalias")) {
            const auto vidPid = ParseVidPid(deviceInterface.at("Modalias").get<std::string>());
            _vendorId = vidPid[0];
            _productId = vidPid[1];
        }

        _uuids = std::vector<std::string>{};
        for (const auto& [interface, properties] : interfaces) {
            auto uuidsIterator = properties.find("UUIDs");
            if (uuidsIterator != properties.end()) {
                auto uuids = uuidsIterator->second.get<std::vector<std::string>>();
                for (const auto& uuid : uuids) {
                    _uuids.push_back(uuid);
                }
            }
        }
        _clazz = deviceInterface.contains("Class")
                    ? std::optional<unsigned int>{ deviceInterface.at("Class").get<unsigned int>() }
                    : std::nullopt;

        _name = deviceInterface.contains("Name")
                    ? deviceInterface.at("Name").get<std::string>()
                    : _address;

        if (deviceInterface.contains("Connected")) {
            _connectionStatus.SetValue(deviceInterface.at("Connected").get<bool>());
        }

        if (deviceInterface.contains("Paired")) {
            _pairedStatus.SetValue(deviceInterface.at("Paired").get<bool>());
        }

        _deviceProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            if (interfaceName == "org.bluez.Device1") {
                if (values.contains("Modalias")) {
                    const auto vidPid = ParseVidPid(values.at("Modalias").get<std::string>());
                    _vendorId = vidPid[0];
                    _productId = vidPid[1];
                }
                if (values.contains("Connected")) {
                    _connectionStatus.SetValue(values["Connected"].get<bool>());
                }
                if (values.contains("Paired")) {
                    _pairedStatus.SetValue(values.at("Paired").get<bool>());
                }
            }
        });
        _deviceProxy->finishRegistration();
    }

    void DBusDeviceInfo::Connect() {
        _deviceProxy->callMethod("Connect").onInterface("org.bluez.Device1").dontExpectReply();
    }

    void DBusDeviceInfo::ConnectAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _deviceProxy->callMethodAsync("Connect").withTimeout(std::chrono::seconds(10)).onInterface("org.bluez.Device1").uponReplyInvoke(callback);
    }

    void DBusDeviceInfo::Disconnect() {
        _deviceProxy->callMethod("Disconnect").onInterface("org.bluez.Device1").dontExpectReply();
    }

    void DBusDeviceInfo::DisconnectAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _deviceProxy->callMethodAsync("Disconnect").withTimeout(std::chrono::seconds(10)).onInterface("org.bluez.Device1").uponReplyInvoke(callback);
    }

    std::array<unsigned short, 2> DBusDeviceInfo::ParseVidPid(const std::string& modalias) {
        std::smatch match;
        std::regex pattern("v([0-9A-Fa-f]{4})p([0-9A-Fa-f]{4})");

        unsigned short vid = 0;
        unsigned short pid = 0;

        if (std::regex_search(modalias, match, pattern)) {
            try {
                vid = static_cast<unsigned short>(std::stoul(match[1], nullptr, 16));
                pid = static_cast<unsigned short>(std::stoul(match[2], nullptr, 16));
            } catch (...) {}
        }

        return {vid, pid};
    }
}