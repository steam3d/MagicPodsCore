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

        UpdateInfos();

        _rootProxy->uponSignal("InterfacesAdded").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath path, std::map<std::string, std::map<std::string, sdbus::Variant>> dictionary) {
            std::cout << "OnInterfacesAdded" << std::endl;

            std::set<std::shared_ptr<Device>, DeviceComparator> addedDevices{};

            auto actualDevices = LoadActualDevices();
            for (const auto& actualDevice : actualDevices) {
                if (!_devices.contains(actualDevice))
                    addedDevices.emplace(actualDevice);
            }

            if (!addedDevices.empty())
                OnDevicesAdd(addedDevices);

            UpdateInfos();
        });

        _rootProxy->uponSignal("InterfacesRemoved").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath path, std::vector<std::string> array) {
            std::cout << "OnInterfacesRemoved" << std::endl;

            std::set<std::shared_ptr<Device>, DeviceComparator> removedDevices{};

            auto actualDevices = LoadActualDevices();
            for (const auto& knownDevice : _devices) {
                if (!actualDevices.contains(knownDevice))
                    removedDevices.emplace(knownDevice);
            }

            if (!removedDevices.empty())
                OnDevicesRemove(removedDevices);

            UpdateInfos();
        });

        _rootProxy->finishRegistration();
    }

    std::set<std::shared_ptr<Device>> DevicesInfoFetcher::GetAirpodsInfos() {
        UpdateInfos();
        
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
        for (auto& device : _devices) {
            if (predicate(*device))
                airpodsDevices.emplace(device);
        }

        return airpodsDevices;
    }

    void DevicesInfoFetcher::Connect(const std::string& deviceAddress) {
        UpdateInfos();
        for (auto& device : _devices) {
            if (device->GetAddress() == deviceAddress) {
                device->Connect();
                break;
            }
        }
    }

    void DevicesInfoFetcher::Disconnect(const std::string& deviceAddress) {
        UpdateInfos();
        for (auto& device : _devices) {
            if (device->GetAddress() == deviceAddress) {
                device->Disconnect();
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

    void DevicesInfoFetcher::UpdateInfos() {
        _devices = LoadActualDevices();
    }

    std::set<std::shared_ptr<Device>, DeviceComparator> DevicesInfoFetcher::LoadActualDevices() {
        std::set<std::shared_ptr<Device>, DeviceComparator> devices{};

        std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects{};
        _rootProxy->callMethod("GetManagedObjects").onInterface("org.freedesktop.DBus.ObjectManager").storeResultsTo<std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>>>(managedObjects);
        for (const auto& [objectPath, interfaces] : managedObjects) {
            const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]/dev(_[0-9A-F]{2}){6}$"};
            std::smatch match;
            
            if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
                if (interfaces.contains("org.bluez.Device1")) {
                    auto deviceInterface = interfaces.at("org.bluez.Device1");
                    auto device = std::make_shared<Device>(objectPath, deviceInterface);
                    devices.emplace(device);
                }
            }
        }

        return devices;
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
