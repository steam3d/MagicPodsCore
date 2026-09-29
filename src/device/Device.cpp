// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "Device.h"
#include "DevicesInfoFetcher.h"

namespace MagicPodsCore {
    void Device::SubscribeCapabilitiesChanges()
    {
        for (auto& c: capabilities){
            size_t id = c->GetChangedEvent().Subscribe([this](size_t id, const Capability &capability)
            {
                _onCapabilityChangedEvent.FireEvent(capability);
                Logger::Debug("%s: capability: %s changed", this->GetName().c_str(), capability.GetName().c_str());
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

    std::string Device::GetContainerName()
    {
        std::string name = GetAddress();
        std::replace(name.begin(), name.end(), ':', '_');
        return name;
    }

    Device::Device(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService) : _deviceInfo{deviceInfo}, _audioClient{audioClient}, _settingsService{settingsService} 
    {
    }

    void Device::SetClientState(ClientState state)
    {
        if (_clientState.exchange(state) != state)
            _onClientStateChangedEvent.FireEvent(state);
    }

    void Device::Init()
    {
        Logger::Info("%s: Init", GetName().c_str());
        SubscribeCapabilitiesChanges();

        if (_client){
            clientReceivedDataEventId = _client->GetOnReceivedDataEvent().Subscribe([this](size_t id, const std::vector<unsigned char> &data)
            {
                // The link carries data, so whatever it took to get here worked
                _clientReconnectAttempts = 0;
                SetClientState(ClientState::Connected);
                OnResponseDataReceived(data);
            });

            clientConnectionLostEventId = _client->GetOnConnectionLostEvent().Subscribe([this](size_t id, const std::string &address)
            {
                Logger::Info("%s: client connection lost while still reported as connected", GetName().c_str());
                RestartClientAfterConnectionLost();
            });
        }

        _deviceHandsFreeBatteryStatusChangedEvent = _deviceInfo->GetHandsFreeBatteryStatus().GetEvent().Subscribe([this](size_t listener_id, uint8_t newBatteryValue) {
            Logger::Debug("%s: PropertiesChanged:HandsFreeBattery %u",GetName().c_str(), newBatteryValue);
            _onHandsFreeBatteryPropertyChangedEvent.FireEvent(newBatteryValue);
        });

        _deviceConnectedStatusChangedEvent = _deviceInfo->GetConnectionStatus().GetEvent().Subscribe([this](size_t listenerId, bool newConnectedValue) {
            if (_connected != newConnectedValue) {
                _connected = newConnectedValue;
                Logger::Debug("%s: PropertiesChanged:Connected %s",GetName().c_str(), _connected ? "true" : "false");
                _onConnectedPropertyChangedEvent.FireEvent(_connected);
            }
            if (_client){
                if (_connected){
                    // Attempts spent on the previous connection are not held against the new one
                    _clientReconnectAttempts = 0;
                    StartClient();
                    Logger::Info("%s _client started from PropertiesChanged", GetName().c_str());
                }
                else{
                    _client->Stop();
                    SetClientState(ClientState::Disconnected);
                    Logger::Info("%s _client stopped from PropertiesChanged", GetName().c_str());
                }
            }
        });

        _connected = _deviceInfo->GetConnectionStatus().GetValue();
        Logger::Debug("%s: Init:Connected %s",GetName().c_str(), _connected ? "true" : "false");
        if (_connected && _client){
            StartClient();
            Logger::Info("%s _client started from Init", GetName().c_str());
        }
    }

    // Called on the reading thread of a dead connection or after a start which did not get through, both leave the device reported as connected with nothing talking to it
    void Device::RestartClientAfterConnectionLost()
    {
        // A disconnect is already on its way and will start the client again by itself.
        if (!_connected) {
            SetClientState(ClientState::Disconnected);
            return;
        }

        SetClientState(ClientState::Reconnecting);

        if (_clientReconnectAttempts >= MAX_CLIENT_RECONNECT_ATTEMPTS){
            Logger::Info("%s: client did not stay connected, dropping client capabilities", GetName().c_str());
            SetClientState(ClientState::Disconnected);
            return;
        }

        // One restart at a time. The link dying and a failed start can both ask for one while a retry is already waiting, the waiting one covers them.
        if (_clientRestartPending.exchange(true))
            return;

        int attempt = ++_clientReconnectAttempts;
        Logger::Info("%s: restarting the client, attempt %d of %d", GetName().c_str(), attempt, MAX_CLIENT_RECONNECT_ATTEMPTS);

        // The wait and the retry run on a thread of their own which holds the device only weakly: the caller is the reader of the dead socket or the DBus dispatcher, neither may block for the second the peer needs,
        // and the device may be gone by the time it has passed. Locking the reference keeps the device alive for the attempt, an expired one means there is nothing left to restart.
        std::thread([weakDevice = weak_from_this()]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto device = weakDevice.lock();
            if (!device)
                return;

            device->_clientRestartPending = false;
            if (!device->_connected)
                return;

            device->StartClient();
        }).detach();
    }

    bool Device::TryStartClient()
    {
        _client->Start([this](Client& client) {
            for (auto& data: this->_clientStartData){
                // Some devices send the start data to discover something and have no use for the rest of it once answered
                if (ShouldStopSendingStartData()){
                    Logger::Debug("%s: start data is no longer needed, skipping the rest", GetName().c_str());
                    break;
                }

                client.SendData(data);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
        });

        return _client->IsStarted();
    }

    void Device::StartClient()
    {
        if (_client->IsStarted()) {
            SetClientState(ClientState::Connected);
            return;
        }

        // The adapter reports the device connected before its RFCOMM service accepts anything, so a start right after a reconnect can fail outright.
        // The calling thread makes one attempt, as it always did, and the retries with their waits are handed off so the DBus dispatcher is never held up by them.
        if (TryStartClient())
            SetClientState(ClientState::Connected);
        else
            RestartClientAfterConnectionLost();
    }

    Device::~Device()
    {
        _deviceInfo->GetConnectionStatus().GetEvent().Unsubscribe(_deviceConnectedStatusChangedEvent);
        _deviceInfo->GetHandsFreeBatteryStatus().GetEvent().Unsubscribe(_deviceHandsFreeBatteryStatusChangedEvent);

        UnsubscribeCapabilitiesChanges();
        capabilities.clear();

        if (_client) {
            _client->Stop();
            _client->GetOnReceivedDataEvent().Unsubscribe(clientReceivedDataEventId);
            _client->GetOnConnectionLostEvent().Unsubscribe(clientConnectionLostEventId);
        }
        Logger::Debug("Device::~Device");

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

    void Device::SaveSettingString(const std::string &settingName, const std::string &value)
    {        
        _settingsService->SaveSetting(GetContainerName(), settingName, value);
    }

    std::optional<std::string> Device::LoadSettingString(const std::string &settingName)
    {
        toml::node_view<toml::node> value = _settingsService->GetSetting(GetContainerName(), settingName);
        if (auto v = value.as_string())
            return v->get();

        return std::nullopt;
    }

    void Device::SaveSettingInt(const std::string &settingName, const int64_t value)
    {
        _settingsService->SaveSetting(GetContainerName(), settingName, value);
    }

    std::optional<int64_t> Device::LoadSettingInt(const std::string &settingName)
    {
        toml::node_view<toml::node> value = _settingsService->GetSetting(GetContainerName(), settingName);
        if (auto v = value.as_integer())
            return v->get();

        return std::nullopt;        
    }

    nlohmann::json Device::GetAsJson()
    {
        auto capabilitiesJson = nlohmann::json::object();
        auto deviceJson = nlohmann::json::object();

        deviceJson["name"] = GetName();
        deviceJson["address"] = GetAddress();
        deviceJson["connected"] = GetConnected();
        deviceJson["model"] = GetProductId();
        deviceJson["vendor"] = GetVendorId();
        
        std::optional<int64_t> settingColor = LoadSettingInt("color");        
        deviceJson["color"] = settingColor.has_value()? settingColor.value() : 0;

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
