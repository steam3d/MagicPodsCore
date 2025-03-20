#include "Device.h"

namespace MagicPodsCore {
    void Device::SubscribeCapabilitiesChanges()
    {
        for (auto& c: capabilities){
            size_t id = c->GetChangedEvent().Subscribe([this](size_t id, const Capability &capability)
            {
                _onCapabilityChangedEvent.FireEvent(capability);
                LOG_DEBUG("Capability changed");
            });
            capabilityEventIds.push_back(id);
        }
    }

    void Device::UnsubscribeCapabilitiesChanges()
    {
        if (capabilityEventIds.size() != capabilities.size())
            throw std::runtime_error("Size of capabilityEventIds and capabilities different");

        for (int i=0; i<capabilities.size(); i++){
            auto& c = capabilities[i];
            c->GetChangedEvent().Unsubscribe(capabilityEventIds[i]);
        }
        capabilityEventIds.clear();
    }

    Device::Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface): _deviceProxy{sdbus::createProxy("org.bluez", objectPath)}
    {
        if (deviceInterface.contains("Name")) {
            _name = deviceInterface.at("Name").get<std::string>();
        }

        if (deviceInterface.contains("Address")) {
            _address = deviceInterface.at("Address").get<std::string>();
        }

        if (deviceInterface.contains("Connected")) {
            _connected = deviceInterface.at("Connected").get<bool>();
            LOG_DEBUG("_connected: %s", _connected ? "true" : "false");
        }

        if (deviceInterface.contains("Modalias")) {
            _modaliasString = deviceInterface.at("Modalias").get<std::string>();
        }

        //TODO parse modalias as Product and Vendor
    }

    void Device::Init()
    {
        LOG_RELEASE("Init");
        SubscribeCapabilitiesChanges();

        clientReceivedDataEventId = _client->GetOnReceivedDataEvent().Subscribe([this](size_t id, const std::vector<unsigned char> &data)
        { OnResponseDataReceived(data); });

        LOG_DEBUG("_connected 0 %s", _connected ? "true" : "false");
        if (_connected){
            _client->Start();
            LOG_RELEASE("_client started 0");
        }
        
        // When the client above is connected, PropertiesChanged will fire twice.
        // First connection false
        // Second connections true
        _deviceProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            //LOG_RELEASE("PropertiesChanged");
            if (values.contains("Connected")) {
                auto newConnectedValue = values["Connected"].get<bool>();
                if (_connected != newConnectedValue) {
                    _connected = newConnectedValue;
                    LOG_DEBUG("PropertiesChanged:Connected %s", _connected ? "true" : "false");
                    _onConnectedPropertyChangedEvent.FireEvent(_connected);
                }
                if (_connected){                    
                    _client->Start(); 
                    LOG_RELEASE("_client started 1");                   
                }
                else{
                    _client->Stop();
                    LOG_RELEASE("_client stopped 1");
                    //throw std::runtime_error("TODO ADD RESETTING CAPABILITIES");                    
                }
            }
        });

        _deviceProxy->finishRegistration();
    }

    Device::~Device()
    {
        _client->GetOnReceivedDataEvent().Unsubscribe(clientReceivedDataEventId);
        UnsubscribeCapabilitiesChanges();
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

    nlohmann::json Device::GetAsJson()
    {
        auto capabilitiesJson = nlohmann::json::object();
        auto deviceJson = nlohmann::json::object();

        deviceJson["name"] = GetName();
        deviceJson["address"] = GetAddress();
        deviceJson["connected"] = GetConnected();

        for (auto& capability : capabilities)
        {
            auto capabilityJson = capability->GetAsJson();
            if (!capabilityJson.empty())
            capabilitiesJson.update(capabilityJson);
        }

        deviceJson["capabilities"] = capabilitiesJson;

        return deviceJson;
    }
}