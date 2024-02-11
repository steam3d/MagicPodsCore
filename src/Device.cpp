#include "Device.h"

namespace MagicPodsCore {

    Device::Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)} {
        if (deviceInterface.contains("Name"))
            _name = deviceInterface.at("Name").get<std::string>();
        if (deviceInterface.contains("Address"))
            _address = deviceInterface.at("Address").get<std::string>();
        if (deviceInterface.contains("Connected"))
            _connected = deviceInterface.at("Connected").get<bool>();
        if (deviceInterface.contains("Modalias"))
            _modalias = deviceInterface.at("Modalias").get<std::string>();
    }

    void Device::Connect() {
        _deviceProxy->callMethod("Connect").onInterface("org.bluez.Device1").dontExpectReply();
    }

    void Device::Disconnect() {
        _deviceProxy->callMethod("Disconnect").onInterface("org.bluez.Device1").dontExpectReply();
    }

}