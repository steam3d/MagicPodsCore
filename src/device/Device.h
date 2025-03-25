#pragma once

#include <sdbus-c++/sdbus-c++.h>

#include "device/capabilities/Capability.h"
#include "client/Client.h"
#include "Event.h"
#include "StringUtils.h"
#include "Logger.h"
#include <iostream>
#include <vector>
#include <json.hpp>

namespace MagicPodsCore {

    class Device {
    private:
        std::unique_ptr<sdbus::IProxy> _deviceProxy{};

        std::string _name{};
        bool _connected{};    
        Event<bool> _onConnectedPropertyChangedEvent{};
        Event<Capability> _onCapabilityChangedEvent{};
        size_t clientReceivedDataEventId;
        virtual void OnResponseDataReceived(const std::vector<unsigned char> &data) = 0;
        void SubscribeCapabilitiesChanges();
        void UnsubscribeCapabilitiesChanges();
        
    protected:
        unsigned short _vendorId = 0;
        unsigned short _productId = 0; //model
        mutable std::mutex _propertyMutex{};
        std::unique_ptr<Client> _client;
        std::string _address{};
        std::vector<std::unique_ptr<Capability>> capabilities{};
        std::vector<size_t> capabilityEventIds{};
        void Init();

    public:
        Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface);
        virtual ~Device(); //wrong
        // TODO: убрать возможность копирования

        std::string GetName() const {
            std::lock_guard lock{_propertyMutex};
            return _name;
        }

        std::string GetAddress() const {
            std::lock_guard lock{_propertyMutex};
            return _address;
        }

        bool GetConnected() const {
            std::lock_guard lock{_propertyMutex};
            return _connected;
        }

        virtual unsigned short GetVendorId() const {
            std::lock_guard lock{_propertyMutex};
            return _vendorId;
        }

        virtual unsigned short GetProductId() const {
            std::lock_guard lock{_propertyMutex};
            return _productId;
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