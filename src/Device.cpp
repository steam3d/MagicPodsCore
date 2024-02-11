#include "Device.h"
#include "StringUtils.h"

#include <iostream>

namespace MagicPodsCore {

    std::shared_ptr<Device> Device::TryCreateDevice(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) {
        const static auto checkModalias = [](const std::string& modalias) -> bool {
            for (auto& appleProductId : AllAppleProductIds) {
                std::string upperCaseActualModalias = modalias;
                std::transform(upperCaseActualModalias.begin(), upperCaseActualModalias.end(), upperCaseActualModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                std::string upperCaseTargetModalias = StringUtils::Format("v%04Xp%04X", static_cast<unsigned short>(BtVendorIds::Apple), static_cast<unsigned short>(appleProductId));
                std::transform(upperCaseTargetModalias.begin(), upperCaseTargetModalias.end(), upperCaseTargetModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                if (upperCaseActualModalias.contains(upperCaseTargetModalias))
                    return true;
            }
            return false;
        };
    
        return deviceInterface.contains("Modalias") && checkModalias(deviceInterface.at("Modalias").get<std::string>()) 
            ? std::make_shared<Device>(objectPath, deviceInterface)
            : nullptr;
    }

    Device::Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)} {
        if (deviceInterface.contains("Name")) {
            _name = deviceInterface.at("Name").get<std::string>();
        }

        if (deviceInterface.contains("Address")) {
            _address = deviceInterface.at("Address").get<std::string>();
            _aapClient = std::make_unique<AapClient>(_address);
            _aapClient->GetBatteryEvent().Subscribe([this](const BatteryWatcherData& data) {
                OnBatteryEvent(data);
            });
            _aapClient->GetAncEvent().Subscribe([this](const AncWatcherData& data) {
                OnAncEvent(data);
            });
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
            std::cout << "PropertiesChanged" << std::endl;
            // for (const auto& [key, value]: values) {
            //     std::cout << "PropertiesChanged:" << key << std::endl;
            // }
            if (values.contains("Connected")) {
                _connected = values["Connected"];
                _onConnectedPropertyChangedEvent.FireEvent(_connected);
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

    void Device::OnBatteryEvent(const BatteryWatcherData& data) {
        std::lock_guard{_propertyMutex};
        _batteryStatus[data.Type] = data;
    }
    
    void Device::OnAncEvent(const AncWatcherData& data) {
        std::lock_guard{_propertyMutex};
        _ancMode = data.Mode;
    }

}