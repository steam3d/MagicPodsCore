#include "DevicesInfoFetcher.h"

#include "btvendorids.h"
#include "appleproductids.h"

#include <regex>
#include <iostream>
#include <format>
#include <algorithm>

namespace MagicPodsCore {

    DevicesInfoFetcher::DevicesInfoFetcher() {
        _rootProxy = sdbus::createProxy("org.bluez", "/");
    }

    std::vector<std::shared_ptr<Device>> DevicesInfoFetcher::GetAirpodsInfos() {
        UpdateInfos();
        
        auto predicate = [](const Device& deviceInfo) {
            for (auto& appleProductId : AllAppleProductIds) {
                std::string upperCaseActualModalias = deviceInfo.GetModalias();
                std::transform(upperCaseActualModalias.begin(), upperCaseActualModalias.end(), upperCaseActualModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                std::string upperCaseTargetModalias = std::format("v{:04x}p{:04x}", static_cast<unsigned short>(BtVendorIds::Apple), static_cast<unsigned short>(appleProductId));
                std::transform(upperCaseTargetModalias.begin(), upperCaseTargetModalias.end(), upperCaseTargetModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                if (upperCaseActualModalias.contains(upperCaseTargetModalias))
                    return true;
            }
            return false;
        };

        std::vector<std::shared_ptr<Device>> airpodsDevices{};
        for (auto& device : _devices) {
            if (predicate(*device))
                airpodsDevices.emplace_back(device);
        }

        return airpodsDevices;
    }

    void DevicesInfoFetcher::UpdateInfos() {
        _devices.clear();

        std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects{};
        _rootProxy->callMethod("GetManagedObjects").onInterface("org.freedesktop.DBus.ObjectManager").storeResultsTo<std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>>>(managedObjects);
        for (const auto& [objectPath, interfaces] : managedObjects) {
            const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]/dev(_[0-9A-F]{2}){6}$"};
            std::smatch match;
            
            if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
                if (interfaces.contains("org.bluez.Device1")) {
                    auto deviceInterface = interfaces.at("org.bluez.Device1");
                    auto device = std::make_shared<Device>(objectPath, deviceInterface);
                    _devices.emplace_back(device);
                }
            }
        }
    }

}
