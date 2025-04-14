// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/capabilities/Capability.h"
#include "client/Client.h"
#include "Event.h"
#include "StringUtils.h"
#include "Logger.h"
#include "dbus/DBusDeviceInfo.h"

#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <vector>
#include <json.hpp>

namespace MagicPodsCore {

    class Device {
    private:
        std::shared_ptr<DBusDeviceInfo> _deviceInfo{};
        
        bool _connected{};
        Event<bool> _onConnectedPropertyChangedEvent{};
        Event<Capability> _onCapabilityChangedEvent{};
        size_t clientReceivedDataEventId;
        size_t _deviceConnectedStatusChangedEvent{};
        virtual void OnResponseDataReceived(const std::vector<unsigned char> &data) = 0;
        void SubscribeCapabilitiesChanges();
        void UnsubscribeCapabilitiesChanges();

    protected:
        mutable std::mutex _propertyMutex{};
        std::unique_ptr<Client> _client;
        std::vector<std::vector<unsigned char>>_clientStartData {};
        std::vector<std::unique_ptr<Capability>> capabilities{};
        std::vector<size_t> capabilityEventIds{};
        void Init();

    public:
        Device(std::shared_ptr<DBusDeviceInfo> deviceInfo);
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

        Event<bool>& GetConnectedPropertyChangedEvent() {
            return _onConnectedPropertyChangedEvent;
        }

        Event<Capability>& GetCapabilityChangedEvent() {
            return _onCapabilityChangedEvent;
        }

        void Connect(); // TODO: может полностью перейти на Async?
        void ConnectAsync(std::function<void(const sdbus::Error*)>&& callback);

        void Disconnect(); // TODO: может полностью перейти на Async?
        void DisconnectAsync(std::function<void(const sdbus::Error*)>&& callback);

        void SetCapabilities(const nlohmann::json &json);

        nlohmann::json GetAsJson();
    };

}