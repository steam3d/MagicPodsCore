#include "./dbus/DBusDeviceInfo.h"

namespace MagicPodsCore {

    DBusDeviceInfo::DBusDeviceInfo(const sdbus::ObjectPath& objectPath, const std::map<std::string, std::map<std::string, sdbus::Variant>>& interfaces) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)}
    {
        auto deviceInterface = interfaces.at("org.bluez.Device1");

        _address = deviceInterface.at("Address").get<std::string>(),
        _modalias = deviceInterface.contains("Modalias") // TODO: modalias
                    ? DBusDeviceModalias{0, 0}
                    : DBusDeviceModalias{std::nullopt, std::nullopt};
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
        _name = deviceInterface.at("Name").get<std::string>();
        _connectionStatus.SetValue(deviceInterface.at("Connected").get<bool>());

        _deviceProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            if (values.contains("Connected")) {
                _connectionStatus.SetValue(values["Connected"].get<bool>());
            }
        });
        _deviceProxy->finishRegistration();
    }
}