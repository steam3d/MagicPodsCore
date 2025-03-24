#include "DevicesInfoFetcher.h"

#include "BtVendorIds.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "StringUtils.h"
#include "Logger.h"

#include <regex>
#include <iostream>
#include <algorithm>
#include "device/GalaxyBudsDevice.h"
#include "device/AapDevice.h"

namespace MagicPodsCore {

    DevicesInfoFetcher::DevicesInfoFetcher() : _rootProxy{sdbus::createProxy("org.bluez", "/")}, _defaultBluetoothAdapterProxy{sdbus::createProxy("org.bluez", "/org/bluez/hci0")} {
        ClearAndFillDevicesMap();

        _rootProxy->uponSignal("InterfacesAdded").onInterface("org.freedesktop.DBus.ObjectManager").call([this](sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces) {
            LOG_DEBUG("OnInterfacesAdded");

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
            LOG_DEBUG("OnInterfacesRemoved");

            std::set<std::shared_ptr<Device>, DeviceComparator> removedDevices{};

            if (_devicesMap.contains(objectPath)) {
                removedDevices.emplace(_devicesMap[objectPath]);
                _devicesMap.erase(objectPath);

            }

            TrySelectNewActiveDevice();

            if (!removedDevices.empty())
                OnDevicesRemove(removedDevices);
        });

        _defaultBluetoothAdapterProxy->uponSignal("PropertiesChanged").onInterface("org.freedesktop.DBus.Properties").call([this](std::string interfaceName, std::map<std::string, sdbus::Variant> values, std::vector<std::string> stringArray) {
            if (values.contains("Powered")) {
                auto newPoweredValue = values["Powered"].get<bool>();
                _isBluetoothAdapterPowered = newPoweredValue;
                _onDefaultAdapterChangeEnabled.FireEvent(_isBluetoothAdapterPowered);
            }
        });

        _rootProxy->finishRegistration();
        _defaultBluetoothAdapterProxy->finishRegistration();

        _isBluetoothAdapterPowered = _defaultBluetoothAdapterProxy->getProperty("Powered").onInterface("org.bluez.Adapter1").get<bool>();
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

    void DevicesInfoFetcher::SetCapabilities(const nlohmann::json &json)
    {        
        LOG_RELEASE("DevicesInfoFetcher::SetCapabilities");

        if (!json.contains("arguments") || !json["arguments"].contains("address") || !json["arguments"].contains("capabilities"))
        {
            LOG_RELEASE("Error: missing required fields in SetCapabilities");
            return;
        }
    
        const auto& arguments = json.at("arguments");
        const auto& deviceAddress = arguments.at("address").get_ref<const std::string&>();
        const auto& capabilities = arguments.at("capabilities");
    
        for (const auto& [key, device] : _devicesMap)
        {
            if (device->GetAddress() == deviceAddress)
            {
                device->SetCapabilities(capabilities);
                break;
            }
        }
    }

    void DevicesInfoFetcher::EnableBluetoothAdapter() {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(true).uponReplyInvoke([this](const sdbus::Error* err) {
            LOG_RELEASE("Adapter enabled");
            _onDefaultAdapterChangeEnabled.FireEvent(IsBluetoothAdapterPowered());
        });
    }

    void DevicesInfoFetcher::EnableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(true).uponReplyInvoke(callback);
    }

    void DevicesInfoFetcher::DisableBluetoothAdapter() {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(false).uponReplyInvoke([this](const sdbus::Error* err) {
            LOG_RELEASE("Adapter disabled");
            _onDefaultAdapterChangeEnabled.FireEvent(IsBluetoothAdapterPowered());
        });
    }

    void DevicesInfoFetcher::DisableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _defaultBluetoothAdapterProxy->setPropertyAsync("Powered").onInterface("org.bluez.Adapter1").toValue(false).uponReplyInvoke(callback);
    }

    std::shared_ptr<Device> DevicesInfoFetcher::TryCreateDevice(const sdbus::ObjectPath& objectPath, const std::map<std::string, sdbus::Variant>& deviceInterface) {        
        const static auto checkModalias = [](const std::string& modalias) -> bool {
            for (auto& appleProductId : AllAapModelsIds) {
                std::string upperCaseActualModalias = modalias;
                std::transform(upperCaseActualModalias.begin(), upperCaseActualModalias.end(), upperCaseActualModalias.begin(), [](unsigned char c){ return std::toupper(c); });
                std::string upperCaseTargetModalias = StringUtils::Format("v%04Xp%04X", static_cast<unsigned short>(BtVendorIds::Apple), static_cast<unsigned short>(appleProductId));
                std::transform(upperCaseTargetModalias.begin(), upperCaseTargetModalias.end(), upperCaseTargetModalias.begin(), [](unsigned char c){ return std::toupper(c); });                


                if (upperCaseActualModalias.contains(upperCaseTargetModalias))
                    return true;
            }
            return false;
        };
            
        const static auto checkHardcodedModalias = [](const std::string& modalias) -> bool {
            
            LOG_RELEASE("%s", modalias.c_str());
            return modalias == "bluetooth:v0075pA013d0001";
        };
    
        if (deviceInterface.contains("Modalias") && checkHardcodedModalias(deviceInterface.at("Modalias").get<std::string>())){
            auto newDevice = GalaxyBudsDevice::Create(objectPath, deviceInterface, 9);
            newDevice->GetConnectedPropertyChangedEvent().Subscribe([this](size_t listenerId, bool newValue) {
                TrySelectNewActiveDevice();
            });
            return newDevice;
        }

        if (deviceInterface.contains("Modalias") && checkModalias(deviceInterface.at("Modalias").get<std::string>())) {
            //auto newDevice = std::make_shared<Device>(objectPath, deviceInterface);
            //auto newDevice = std::make_shared<AapDevice>(objectPath, deviceInterface);
            auto newDevice = AapDevice::Create(objectPath, deviceInterface);
            newDevice->GetConnectedPropertyChangedEvent().Subscribe([this](size_t listenerId, bool newValue) {
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

        LOG_RELEASE("Devices created: %zu", _devicesMap.size());
    }

    void DevicesInfoFetcher::TrySelectNewActiveDevice() {
        auto previousActiveDevice = _activeDevice;

        if (_activeDevice != nullptr && (!_devicesMap.contains(_activeDevice->GetAddress()) || !_activeDevice->GetConnected()))
            _activeDevice = nullptr;

        if (_activeDevice == nullptr && _devicesMap.size() > 0) {
            for (auto& [address, device] : _devicesMap) {
                if (device->GetConnected())
                    _activeDevice = device;
            }
        }

        if (previousActiveDevice != _activeDevice)
            _onActiveDeviceChangedEvent.FireEvent(_activeDevice);
    }

    void DevicesInfoFetcher::OnDevicesAdd(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices) {
        LOG_RELEASE("--- OnDevicesAdd ---");
        for (const auto& device : devices)
            LOG_RELEASE("%s %s",  device->GetName().c_str(), device->GetAddress().c_str());

        _onDevicesAddEvent.FireEvent(devices);
    }

    void DevicesInfoFetcher::OnDevicesRemove(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices) {
        LOG_RELEASE("--- OnDevicesRemove ---");
        for (const auto& device : devices)
            LOG_RELEASE("%s %s",  device->GetName().c_str(), device->GetAddress().c_str());

        _onDevicesRemoveEvent.FireEvent(devices);
    }

}
