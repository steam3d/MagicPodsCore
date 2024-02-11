#include "DevicesInfoFetcher.h"

#include "BtVendorIds.h"
#include "AppleProductIds.h"
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
                        if (auto device = Device::TryCreateDevice(objectPath, deviceInterface)) {
                            _devicesMap.emplace(objectPath, device);
                            addedDevices.emplace(device);
                        }
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

    std::set<std::shared_ptr<Device>, DeviceComparator> DevicesInfoFetcher::GetDevices() const {
        std::set<std::shared_ptr<Device>, DeviceComparator> devices{};
        for (const auto& [key, value] : _devicesMap) {
            devices.emplace(value);
        }
        return devices;
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
        auto jsonArray = nlohmann::json::array();
        for (const auto& [key, device] : _devicesMap) {
            auto jsonObject = nlohmann::json::object();
            jsonObject["name"] = device->GetName();
            jsonObject["address"] = device->GetAddress();
            jsonObject["connected"] = device->GetConnected();

            auto jsonBatteryObject = nlohmann::json::object();
            for (const auto& [batteryKey, battery] : device->GetBatteryStatus()) {
                switch (batteryKey) {
                    case BatteryType::Single:
                        jsonBatteryObject["s"] = battery.Battery;
                        jsonBatteryObject["sc"] = battery.Status == ChargingStatus::Charging;
                        break;

                    case BatteryType::Right:
                        jsonBatteryObject["r"] = battery.Battery;
                        jsonBatteryObject["rc"] = battery.Status == ChargingStatus::Charging;
                        break;

                    case BatteryType::Left:
                        jsonBatteryObject["l"] = battery.Battery;
                        jsonBatteryObject["lc"] = battery.Status == ChargingStatus::Charging;
                        break;

                    case BatteryType::Case:
                        jsonBatteryObject["c"] = battery.Battery;
                        jsonBatteryObject["cc"] = battery.Status == ChargingStatus::Charging;
                        break;
                }
            }
            jsonObject["battery"] = jsonBatteryObject;

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
                    if (auto device = Device::TryCreateDevice(objectPath, deviceInterface))
                        _devicesMap.emplace(objectPath, device);
                }
            }
        }

        std::cout << "Devices created:" << _devicesMap.size() << std::endl;
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
