#include "DevicesInfoFetcher.h"

#include "btvendorids.h"
#include "appleproductids.h"
#include "StringUtils.h"

#include <regex>
#include <iostream>
#include <algorithm>

namespace MagicPodsCore {

    DevicesInfoFetcher::DevicesInfoFetcher() {
        _rootProxy = sdbus::createProxy("org.bluez", "/");

        ClearAndFillDevicesMap();

        _rootProxy->uponSignal("InterfacesAdded").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces) {
            std::cout << "OnInterfacesAdded" << std::endl;

            std::set<std::shared_ptr<Device>, DeviceComparator> addedDevices{};

            const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]/dev(_[0-9A-F]{2}){6}$"};
            std::smatch match;
            if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
                if (interfaces.contains("org.bluez.Device1")) {
                    auto deviceInterface = interfaces.at("org.bluez.Device1");
                    
                    if (!_devicesMap.contains(objectPath)) {
                        auto device = std::make_shared<Device>(objectPath, deviceInterface);
                        _devicesMap.emplace(objectPath, device);
                        addedDevices.emplace(device);
                    }
                }
            }

            if (!addedDevices.empty())
                OnDevicesAdd(addedDevices);
        });

        _rootProxy->uponSignal("InterfacesRemoved").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::vector<std::string> array) {
            std::cout << "OnInterfacesRemoved" << std::endl;

            std::set<std::shared_ptr<Device>, DeviceComparator> removedDevices{};

            if (_devicesMap.contains(objectPath)) {
                removedDevices.emplace(_devicesMap[objectPath]);
                _devicesMap.erase(objectPath);

            }

            if (!removedDevices.empty())
                OnDevicesRemove(removedDevices);
        });

        _rootProxy->finishRegistration();
    }

    std::set<std::shared_ptr<Device>> DevicesInfoFetcher::GetAirpodsInfos() {     
        auto predicate = [](const Device& deviceInfo) {
            for (auto& appleProductId : AllAppleProductIds) {
                std::string upperCaseActualModalias = deviceInfo.GetModalias();
                std::transform(upperCaseActualModalias.begin(), upperCaseActualModalias.end(), upperCaseActualModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                std::string upperCaseTargetModalias = StringUtils::Format("v%04Xp%04X", static_cast<unsigned short>(BtVendorIds::Apple), static_cast<unsigned short>(appleProductId));
                std::transform(upperCaseTargetModalias.begin(), upperCaseTargetModalias.end(), upperCaseTargetModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                if (upperCaseActualModalias.contains(upperCaseTargetModalias))
                    return true;
            }
            return false;
        };

        std::set<std::shared_ptr<Device>> airpodsDevices{};
        for (const auto& [key, value] : _devicesMap) {
            if (predicate(*value))
                airpodsDevices.emplace(value);
        }

        return airpodsDevices;
    }

    void DevicesInfoFetcher::Connect(const std::string& deviceAddress) {
        for (const auto& [key, value] : _devicesMap) {
            if (value->GetAddress() == deviceAddress) {
                value->Connect();
                break;
            }
        }
    }

    void DevicesInfoFetcher::Disconnect(const std::string& deviceAddress) {
        for (const auto& [key, value] : _devicesMap) {
            if (value->GetAddress() == deviceAddress) {
                value->Disconnect();
                break;
            }
        }
    }

    std::string DevicesInfoFetcher::AsJson() {
        auto airpodsDevices = GetAirpodsInfos();
        
        auto jsonArray = nlohmann::json::array();
        for (auto& device : airpodsDevices) {
            auto jsonObject = nlohmann::json::object();
            jsonObject["name"] = device->GetName();
            jsonObject["address"] = device->GetAddress();
            jsonObject["connected"] = device->GetConnected();
            jsonArray.push_back(jsonObject);
        }
        return jsonArray.dump();
    }

    void DevicesInfoFetcher::ClearAndFillDevicesMap() {
        _devicesMap.clear();

        std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects{};
        _rootProxy->callMethod("GetManagedObjects").onInterface("org.freedesktop.DBus.ObjectManager").storeResultsTo<std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>>>(managedObjects);
        for (const auto& [objectPath, interfaces] : managedObjects) {
            const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]/dev(_[0-9A-F]{2}){6}$"};
            std::smatch match;
            
            if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
                if (interfaces.contains("org.bluez.Device1")) {
                    auto deviceInterface = interfaces.at("org.bluez.Device1");
                    auto device = std::make_shared<Device>(objectPath, deviceInterface);
                    _devicesMap.emplace(objectPath, device);
                }
            }
        }
    }

    void DevicesInfoFetcher::OnDevicesAdd(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices) {
        std::cout << "--- OnDevicesAdd ---" << std::endl;
        for (const auto& device : devices)
            std::cout << device->GetName() << " " << device->GetAddress() << std::endl;

        _onDevicesAddEvent.FireEvent(devices);
    }

    void DevicesInfoFetcher::OnDevicesRemove(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices) {
        std::cout << "--- OnDevicesRemove ---" << std::endl;
        for (const auto& device : devices)
            std::cout << device->GetName() << " " << device->GetAddress() << std::endl;

        _onDevicesRemoveEvent.FireEvent(devices);
    }

}
