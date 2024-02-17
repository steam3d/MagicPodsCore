#include "Device.h"
#include "StringUtils.h"

#include <iostream>

namespace MagicPodsCore {

    Device::Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)}, _battery(true) {
        if (deviceInterface.contains("Name")) {           
            _name = deviceInterface.at("Name").get<std::string>();
        }

        if (deviceInterface.contains("Address")) {
            _address = deviceInterface.at("Address").get<std::string>();
            _aapClient = std::make_unique<AapClient>(_address);
            _aapClient->GetBatteryEvent().Subscribe([this](const std::map<BatteryType, BatteryWatcherData>& data) {
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
                auto newConnectedValue = values["Connected"].get<bool>();
                if (_connected != newConnectedValue) {
                    _connected = newConnectedValue;
                    _onConnectedPropertyChangedEvent.FireEvent(_connected);
                }
                if (_connected)
                    _aapClient->Start();
                else{
                    _aapClient->Stop();
                    _battery.ClearBattery();
                    _anc.ClearAnc();
                }
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

    void Device::SetAnc(DeviceAncMode mode) {
        if (_aapClient->IsStarted())
            _aapClient->SendRequest(AapSetAnc(_anc.DeviceAncModeAncModeTo(mode)));        
    }

    void Device::OnBatteryEvent(const std::map<BatteryType, BatteryWatcherData>& data) {
        std::lock_guard{_propertyMutex};
        _battery.UpdateFromAppleBattery(data);
    }
    
    void Device::OnAncEvent(const AncWatcherData& data) {
        std::lock_guard{_propertyMutex};
        _anc.UpdateFromAppleAnc(data.Mode);
    }

}