#include "DevicesInfoFetcher.h"

#include "BtVendorIds.h"
#include "AppleProductIds.h"
#include "StringUtils.h"

#include <regex>
#include <iostream>
#include <algorithm>

namespace MagicPodsCore {

    DevicesInfoFetcher::DevicesInfoFetcher() : _rootProxy{sdbus::createProxy("org.bluez", "/")}, _defaultBluetoothAdapterProxy{sdbus::createProxy("org.bluez", "/org/bluez/hci0")} {
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
                        if (auto device = TryCreateDevice(objectPath, deviceInterface)) {
                            _devicesMap.emplace(objectPath, device);
                            addedDevices.emplace(device);
                        }
                    }
                }
            }

            TrySelectNewActiveDevice();

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

            TrySelectNewActiveDevice();

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

    std::shared_ptr<Device> DevicesInfoFetcher::GetDevice(std::string& deviceAddress) const {
        for (const auto& [key, value] : _devicesMap) {
            if (value->GetAddress() == deviceAddress) {
                return value;
            }
        }
        return nullptr;
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

    void DevicesInfoFetcher::SetAnc(const std::string& deviceAddress, DeviceAncMode mode) {
        for (const auto& [key, value] : _devicesMap) {
            if (value->GetAddress() == deviceAddress) {
                value->SetAnc(mode);
                break;
            }
        }
    }

    bool DevicesInfoFetcher::IsBluetoothAdapterPowered() {
        return _defaultBluetoothAdapterProxy->getProperty("Powered").onInterface("org.bluez.Adapter1").get<bool>();
    }

    void DevicesInfoFetcher::EnableBluetoothAdapter() {
        _defaultBluetoothAdapterProxy->setProperty("Powered").onInterface("org.bluez.Adapter1").toValue(true);
    }

    void DevicesInfoFetcher::DisableBluetoothAdapter() {
        _defaultBluetoothAdapterProxy->setProperty("Powered").onInterface("org.bluez.Adapter1").toValue(false);
    }

    std::shared_ptr<Device> DevicesInfoFetcher::TryCreateDevice(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) {
        const static auto checkModalias = [](const std::string& modalias) -> bool {
            for (auto& appleProductId : AllAppleProductIds) {
                std::string upperCaseActualModalias = modalias;
                std::transform(upperCaseActualModalias.begin(), upperCaseActualModalias.end(), upperCaseActualModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                std::string upperCaseTargetModalias = StringUtils::Format("v%04Xp%04X", static_cast<unsigned short>(BtVendorIds::Apple), static_cast<unsigned short>(appleProductId));
                std::transform(upperCaseTargetModalias.begin(), upperCaseTargetModalias.end(), upperCaseTargetModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                if (upperCaseActualModalias.contains(upperCaseTargetModalias))
                    return true;
            }
            return false;
        };
    
        if (deviceInterface.contains("Modalias") && checkModalias(deviceInterface.at("Modalias").get<std::string>())) {
            auto newDevice = std::make_shared<Device>(objectPath, deviceInterface);
            newDevice->GetConnectedPropertyChangedEvent().Subscribe([this](bool newValue) {
                TrySelectNewActiveDevice();
            });
            return newDevice;
        }
        return nullptr;
    }

    void DevicesInfoFetcher::ClearAndFillDevicesMap() {
        _devicesMap.clear();
        _activeDevice = nullptr;

        std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects{};
        _rootProxy->callMethod("GetManagedObjects").onInterface("org.freedesktop.DBus.ObjectManager").storeResultsTo<std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>>>(managedObjects);
        for (const auto& [objectPath, interfaces] : managedObjects) {
            const std::regex DEVICE_INSTANCE_RE{"^/org/bluez/hci[0-9]/dev(_[0-9A-F]{2}){6}$"};
            std::smatch match;
            
            if (std::regex_match(objectPath, match, DEVICE_INSTANCE_RE)) {
                if (interfaces.contains("org.bluez.Device1")) {
                    auto deviceInterface = interfaces.at("org.bluez.Device1");
                    if (auto device = TryCreateDevice(objectPath, deviceInterface))
                        _devicesMap.emplace(objectPath, device);
                }
            }
        }

        TrySelectNewActiveDevice();

        std::cout << "Devices created:" << _devicesMap.size() << std::endl;
        
    }

    void DevicesInfoFetcher::TrySelectNewActiveDevice() {
        if (_activeDevice != nullptr && (!_devicesMap.contains(_activeDevice->GetAddress()) || !_activeDevice->GetConnected()))
            _activeDevice = nullptr;

        if (_activeDevice == nullptr && _devicesMap.size() > 0) {
            for (auto& [address, device] : _devicesMap) {
                if (device->GetConnected())
                    _activeDevice = device;
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
