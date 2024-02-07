#include "Device.h"

#include <iostream>

namespace MagicPodsCore {

    Device::Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)} {
        if (deviceInterface.contains("Name")) {
            _name = deviceInterface.at("Name").get<std::string>();
        }

        if (deviceInterface.contains("Address")) {
            _address = deviceInterface.at("Address").get<std::string>();
            _aapClient = std::make_unique<AapClient>(_address);
        }

        if (deviceInterface.contains("Connected")) {
            _connected = deviceInterface.at("Connected").get<bool>();
            if (_connected)
                _aapClient->Start();
        }

        if (deviceInterface.contains("Modalias")) {
            _modalias = deviceInterface.at("Modalias").get<std::string>();
        }

        _deviceProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            for (const auto& [key, value]: values) {
                std::cout << "PropertiesChanged:" << key << std::endl;
            }
            if (values.contains("Connected")) {
                _onConnectedPropertyChangedEvent.FireEvent(values["Connected"]);
                if (_connected)
                    _aapClient->Start();
                else
                    _aapClient->Stop();
            }
        });
        _deviceProxy->finishRegistration();
    }

    void Device::Connect() {
        _deviceProxy->callMethod("Connect").onInterface("org.bluez.Device1").dontExpectReply();
    }

    void Device::Disconnect() {
        _deviceProxy->callMethod("Disconnect").onInterface("org.bluez.Device1").dontExpectReply();
    }

}