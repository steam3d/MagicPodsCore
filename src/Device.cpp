#include "Device.h"

#include "StringUtils.h"
#include "Logger.h"

#include <iostream>

namespace MagicPodsCore {

    Device::Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)}, _battery(true) {
        if (deviceInterface.contains("Name")) {           
            _name = deviceInterface.at("Name").get<std::string>();
        }

        if (deviceInterface.contains("Address")) {
            _address = deviceInterface.at("Address").get<std::string>();
            _aapClient = std::make_unique<AapClient>(_address);
            _aapClient->GetBatteryEvent().Subscribe([this](size_t listenerId, const std::vector<DeviceBatteryData>& data) {
                OnBatteryEvent(data);
            });
            _aapClient->GetAncEvent().Subscribe([this](size_t listenerId, const AncWatcherData& data) {
                OnAncEvent(data);
            });
        }

        if (deviceInterface.contains("Connected")) {
            _connected = deviceInterface.at("Connected").get<bool>();
            if (_connected)
                _aapClient->Start();
        }

        if (deviceInterface.contains("Modalias")) {
            _modaliasString = deviceInterface.at("Modalias").get<std::string>();
        }

        //TODO parse modalias as Product and Vendor

        _deviceProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            LOG_RELEASE("PropertiesChanged");
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

    void Device::ConnectAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _deviceProxy->callMethodAsync("Connect").withTimeout(std::chrono::seconds(10)).onInterface("org.bluez.Device1").uponReplyInvoke(callback);
    }

    void Device::Disconnect() {
        _deviceProxy->callMethod("Disconnect").onInterface("org.bluez.Device1").dontExpectReply();
    }

    void Device::DisconnectAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _deviceProxy->callMethodAsync("Disconnect").withTimeout(std::chrono::seconds(10)).onInterface("org.bluez.Device1").uponReplyInvoke(callback);
    }

    void Device::SetAnc(DeviceAncMode mode) {
        if (_aapClient->IsStarted())
            _aapClient->SendRequest(AapSetAnc(_anc.DeviceAncModeAncModeTo(mode)));        
    }

    void Device::OnBatteryEvent(const std::vector<DeviceBatteryData>& data) {
        std::lock_guard{_propertyMutex};
        _battery.UpdateBattery(data);
    }
    
    void Device::OnAncEvent(const AncWatcherData& data) {
        std::lock_guard{_propertyMutex};
        _anc.UpdateFromAppleAnc(data.Mode);
    }

}