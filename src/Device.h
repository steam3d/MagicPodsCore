#pragma once

#include <sdbus-c++/sdbus-c++.h>

#include "Event.h"
#include "AapClient.h"

namespace MagicPodsCore {

    class Device {
    private:
        std::unique_ptr<sdbus::IProxy> _deviceProxy{};

        std::string _name{};
        std::string _address{};
        bool _connected{};
        std::string _modalias{};

        Event<bool> _onConnectedPropertyChangedEvent{};

        std::unique_ptr<AapClient> _aapClient{};

    public:
        Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface);
        // TODO: убрать возможность копирования

        std::string GetName() const {
            return _name;
        }

        std::string GetAddress() const {
            return _address;
        }

        bool GetConnected() const {
            return _connected;
        }
        
        std::string GetModalias() const {
            return _modalias;
        }

        Event<bool>& GetConnectedPropertyChangedEvent() {
            return _onConnectedPropertyChangedEvent;
        }

        void Connect();

        void Disconnect();
    };

}