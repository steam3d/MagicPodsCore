#include "Device.h"
#include "DevicesInfoFetcher.h"

namespace MagicPodsCore {
    void Device::SubscribeCapabilitiesChanges()
    {
        for (auto& c: capabilities){
            size_t id = c->GetChangedEvent().Subscribe([this](size_t id, const Capability &capability)
            {
                _onCapabilityChangedEvent.FireEvent(capability);
                LOG_DEBUG("Capability: %s changed", capability.GetName().c_str());
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

    Device::Device(std::shared_ptr<DBusDeviceInfo> deviceInfo) : _deviceInfo{deviceInfo} 
    {
    }

    void Device::Init()
    {
        LOG_RELEASE("Init");
        SubscribeCapabilitiesChanges();

        clientReceivedDataEventId = _client->GetOnReceivedDataEvent().Subscribe([this](size_t id, const std::vector<unsigned char> &data)
        { OnResponseDataReceived(data); });

        _deviceConnectedStatusChangedEvent = _deviceInfo->GetConnectionStatus().GetEvent().Subscribe([this](size_t listenerId, bool newConnectedValue) {
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
            }
        });

        LOG_DEBUG("_connected 0 %s", _connected ? "true" : "false");
        if (_connected){
            _client->Start();
            LOG_RELEASE("_client started 0");
        }
    }

    Device::~Device()
    {
        _deviceInfo->GetConnectionStatus().GetEvent().Unsubscribe(_deviceConnectedStatusChangedEvent);

        UnsubscribeCapabilitiesChanges();
        capabilities.clear();

        _client->GetOnReceivedDataEvent().Unsubscribe(clientReceivedDataEventId);

        //TODO: Unsubscribe all listeners from all events in device. See the event.h
    }

    void Device::Connect() {
        _deviceInfo->Connect();
    }

    void Device::ConnectAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _deviceInfo->ConnectAsync(std::move(callback));
    }

    void Device::Disconnect() {
        _deviceInfo->Disconnect();
    }

    void Device::DisconnectAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _deviceInfo->DisconnectAsync(std::move(callback));
    }

    void Device::SetCapabilities(const nlohmann::json &json)
    {
        for (auto& capability : capabilities)
        {
            capability->SetFromJson(json);
        }
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