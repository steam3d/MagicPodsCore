#include "./dbus/DBusDeviceInfo.h"

namespace MagicPodsCore {

    DBusDeviceInfo::DBusDeviceInfo(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& interface) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)}
    {
        _address = interface.at("Address").get<std::string>(),
        _modalias = interface.contains("Modalias") // TODO: modalias
                    ? DBusDeviceModalias{0, 0}
                    : DBusDeviceModalias{std::nullopt, std::nullopt};
        _uuids = std::vector<std::string>{};
        _clazz = interface.contains("Class") 
                    ? std::optional<unsigned int>{ interface.at("Class").get<unsigned int>() } 
                    : std::nullopt;
        _name = interface.at("Name").get<std::string>();
        _connectionStatus.SetValue(interface.at("Connected").get<bool>());

        _deviceProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            if (values.contains("Connected")) {
                _connectionStatus.SetValue(values["Connected"].get<bool>());
            }
        });
        _deviceProxy->finishRegistration();
    }
}