// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/capabilities/Capability.h"
#include "client/Client.h"
#include "client/ClientState.h"
#include "Event.h"
#include "StringUtils.h"
#include "Logger.h"
#include "dbus/DBusDeviceInfo.h"
#include "pulseaudio/PulseAudioClient.h"
#include "media/MediaController.h"
#include "settings/SettingsService.h"

#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include <optional>
#include <atomic>
#include <memory>

namespace MagicPodsCore {

    // Shared ownership is required: the deferred client restart holds the device through a weak reference so it can never run against a destroyed one.
    class Device : public std::enable_shared_from_this<Device> {
    private:
        // How many times the client is restarted after it goes down without a disconnect to explain it
        static constexpr int MAX_CLIENT_RECONNECT_ATTEMPTS = 12;

        std::shared_ptr<DBusDeviceInfo> _deviceInfo{};
        std::shared_ptr<PulseAudioClient> _audioClient{};
        std::shared_ptr<SettingsService> _settingsService{};
        std::shared_ptr<MediaController> _mediaController{};
        // Read by the deferred restart off the DBus thread, so it is atomic like the attempt counter.
        std::atomic<bool> _connected{};
        Event<bool> _onConnectedPropertyChangedEvent{};
        std::atomic<ClientState> _clientState{ClientState::Disconnected};
        Event<ClientState> _onClientStateChangedEvent{};
        Event<Capability> _onCapabilityChangedEvent{};
        Event<uint8_t> _onHandsFreeBatteryPropertyChangedEvent{};
        size_t clientReceivedDataEventId;
        size_t clientConnectionLostEventId{};
        std::atomic<int> _clientReconnectAttempts{0};
        std::atomic<bool> _clientRestartPending{false};
        size_t _deviceConnectedStatusChangedEvent{};
        size_t _deviceHandsFreeBatteryStatusChangedEvent{};
        virtual void OnResponseDataReceived(const std::vector<unsigned char> &data) = 0;
        void SubscribeCapabilitiesChanges();
        void UnsubscribeCapabilitiesChanges();
        bool TryStartClient();
        void StartClient();
        void RestartClientAfterConnectionLost();
        void SetClientState(ClientState state);

    protected:
        // The device's own settings container name (address-derived). Capabilities that
        // persist per-device options read and write through it via the Load/SaveSetting* API.
        std::string GetContainerName();

        mutable std::mutex _propertyMutex{};
        std::unique_ptr<Client> _client;
        std::vector<std::vector<unsigned char>>_clientStartData {};
        std::vector<std::unique_ptr<Capability>> capabilities{};
        std::vector<size_t> capabilityEventIds{};
        void Init();
        // Lets a device stop the start data early once it served its purpose.
        virtual bool ShouldStopSendingStartData() const { return false; }

    public:
        Device(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, std::shared_ptr<MediaController> mediaController);
        virtual ~Device(); //wrong
        // TODO: убрать возможность копирования

        const std::string& GetName() const {
            std::lock_guard lock{_propertyMutex};
            return _deviceInfo->GetName();
        }

        const std::string& GetAddress() const {
            std::lock_guard lock{_propertyMutex};
            return _deviceInfo->GetAddress();
        }

        bool GetConnected() const {
            std::lock_guard lock{_propertyMutex};
            return _deviceInfo->GetConnectionStatus().GetValue();
        }

        virtual unsigned short GetVendorId() const {
            std::lock_guard lock{_propertyMutex};
            return _deviceInfo->GetVendorId();
        }

        virtual unsigned short GetProductId() const {
            std::lock_guard lock{_propertyMutex};
            return _deviceInfo->GetProductId();
        }

        // True when the device exposes the SMEP simultaneous-multipoint plane that Auto Switch
        // drives (the Galaxy Buds line from Buds2 Pro up). Answered per model by
        // GalaxyBudsDevice; every other device is false and its Auto Switch stays inert.
        virtual bool GetSmepMultipointSupported() const
        {
            return false;
        }

        uint8_t GetHandsFreeBattery() const {
            std::lock_guard lock{_propertyMutex};
            return _deviceInfo->GetHandsFreeBatteryStatus().GetValue();
        }

        std::shared_ptr<PulseAudioClient> GetAudioClient() const {
            std::lock_guard lock{_propertyMutex};
            return _audioClient;
        }

        std::shared_ptr<SettingsService> GetSettingsService() const {
            std::lock_guard lock{_propertyMutex};
            return _settingsService;
        }

        void PauseMedia()
        {
            _mediaController->Pause();
        }

        void PlayMedia()
        {
            _mediaController->Play();
        }

        ClientState GetClientState() const {
            return _clientState;
        }

        Event<ClientState>& GetClientStateChangedEvent() {
            return _onClientStateChangedEvent;
        }

        Event<bool>& GetConnectedPropertyChangedEvent() {
            return _onConnectedPropertyChangedEvent;
        }

        Event<Capability>& GetCapabilityChangedEvent() {
            return _onCapabilityChangedEvent;
        }

        Event<uint8_t>& GetHandsFreeBatteryPropertyChangedEvent() {
            return _onHandsFreeBatteryPropertyChangedEvent;
        }


        void Connect(); // TODO: может полностью перейти на Async?
        void ConnectAsync(std::function<void(const sdbus::Error*)>&& callback);

        void Disconnect(); // TODO: может полностью перейти на Async?
        void DisconnectAsync(std::function<void(const sdbus::Error*)>&& callback);

        void SetCapabilities(const nlohmann::json &json);
        void SendRawData(const std::vector<unsigned char>& data);

        void SaveSettingString(const std::string &settingName, const std::string &value);
        std::optional<std::string> LoadSettingString(const std::string &settingName);
        
        void SaveSettingInt(const std::string &settingName, const int64_t value);
        std::optional<int64_t> LoadSettingInt(const std::string &settingName);

        void SaveSettingBool(const std::string &settingName, const bool value);
        std::optional<bool> LoadSettingBool(const std::string &settingName);

        nlohmann::json GetAsJson();
    };

}
