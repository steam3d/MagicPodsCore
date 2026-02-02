// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/capabilities/Capability.h"
#include "client/Client.h"
#include "Event.h"
#include "StringUtils.h"
#include "Logger.h"
#include "dbus/DBusDeviceInfo.h"
#include "pulseaudio/PulseAudioClient.h"
#include "settings/SettingsService.h"

#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include <optional>

namespace MagicPodsCore {

    class Device {
    private:
        std::shared_ptr<DBusDeviceInfo> _deviceInfo{};
        std::shared_ptr<PulseAudioClient> _audioClient{};
        std::shared_ptr<SettingsService> _settingsService{};
        bool _connected{};
        Event<bool> _onConnectedPropertyChangedEvent{};
        Event<Capability> _onCapabilityChangedEvent{};
        Event<uint8_t> _onHandsFreeBatteryPropertyChangedEvent{};
        size_t clientReceivedDataEventId;
        size_t _deviceConnectedStatusChangedEvent{};
        size_t _deviceHandsFreeBatteryStatusChangedEvent{};
        virtual void OnResponseDataReceived(const std::vector<unsigned char> &data) = 0;
        void SubscribeCapabilitiesChanges();
        void UnsubscribeCapabilitiesChanges();
        std::string GetContainerName();

    protected:
        mutable std::mutex _propertyMutex{};
        std::unique_ptr<Client> _client;
        std::vector<std::vector<unsigned char>>_clientStartData {};
        std::vector<std::unique_ptr<Capability>> capabilities{};
        std::vector<size_t> capabilityEventIds{};
        void Init();

    public:
        Device(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService);
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

        uint8_t GetHandsFreeBattery() const {
            std::lock_guard lock{_propertyMutex};
            return _deviceInfo->GetHandsFreeBatteryStatus().GetValue();
        }

        std::shared_ptr<PulseAudioClient> GetAudioClient() const {
            std::lock_guard lock{_propertyMutex};
            return _audioClient;
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

        void SaveSettingString(const std::string &settingName, const std::string &value);
        std::optional<std::string> LoadSettingString(const std::string &settingName);

        nlohmann::json GetAsJson();
    };

}