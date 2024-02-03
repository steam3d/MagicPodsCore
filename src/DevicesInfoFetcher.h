#pragma once

#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <map>

#include <sdbus-c++/sdbus-c++.h>
#include <json.hpp>

namespace MagicPodsCore {

    class Device {
    private:
        std::unique_ptr<sdbus::IProxy> _deviceProxy{};

        std::string _name{};
        std::string _address{};
        bool _connected{};
        std::string _modalias{};       

    public:
        Device(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) : _deviceProxy{sdbus::createProxy("org.bluez", objectPath)} {
            if (deviceInterface.contains("Name"))
                _name = deviceInterface.at("Name").get<std::string>();
            if (deviceInterface.contains("Address"))
                _address = deviceInterface.at("Address").get<std::string>();
            if (deviceInterface.contains("Connected"))
                _connected = deviceInterface.at("Connected").get<bool>();
            if (deviceInterface.contains("Modalias"))
                _modalias = deviceInterface.at("Modalias").get<std::string>();
        }

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

        void Connect() {
            _deviceProxy->callMethod("Connect").onInterface("org.bluez.Device1").dontExpectReply();
        }

        void Disconnect() {
            _deviceProxy->callMethod("Disconnect").onInterface("org.bluez.Device1").dontExpectReply();
        }
    };

    class DevicesInfoFetcher {
    private:
        std::unique_ptr<sdbus::IProxy> _rootProxy{};
        std::vector<std::shared_ptr<Device>> _devices{};

    public:
        DevicesInfoFetcher();
        // TODO: запретить копирование и перенос

        std::vector<std::shared_ptr<Device>> GetAirpodsInfos();

        std::string AsJson() {
            auto airpodsDevices = GetAirpodsInfos();
            
            auto jsonArray = nlohmann::json::array();
            for (size_t i = 0; i < airpodsDevices.size(); ++i) {
                auto jsonObject = nlohmann::json::object();
                jsonObject["name"] = airpodsDevices[i]->GetName();
                jsonObject["address"] = airpodsDevices[i]->GetAddress();
                jsonObject["connected"] = airpodsDevices[i]->GetConnected();
                jsonArray.push_back(jsonObject);
            }
            return jsonArray.dump();
        }

    private:
        void UpdateInfos();
    };

}
