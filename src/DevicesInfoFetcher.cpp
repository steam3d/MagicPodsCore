#include "DevicesInfoFetcher.h"

#include "BtVendorIds.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/AapHelper.h"
#include "StringUtils.h"
#include "Logger.h"

#include <regex>
#include <iostream>
#include <algorithm>
#include "device/GalaxyBudsDevice.h"
#include "device/AapDevice.h"
#include <regex>


namespace MagicPodsCore {

    DevicesInfoFetcher::DevicesInfoFetcher() {
        ClearAndFillDevicesMap();

        _dbusService.GetOnDeviceAddedEvent().Subscribe([this](size_t listenerId, const std::shared_ptr<DBusDeviceInfo>& addedDeviceInfo) {
            LOG_DEBUG("OnDeviceAdded: %s", addedDeviceInfo->GetAddress().c_str());
            if (!_devicesMap.contains(addedDeviceInfo->GetAddress())) {
                if (auto device = TryCreateDevice(addedDeviceInfo)) {
                    _devicesMap.emplace(addedDeviceInfo->GetAddress(), device);
                    _onDeviceAddEvent.FireEvent(device);
                }
                // TODO: уведомление о добавлении устройства
            }
        });

        _dbusService.GetOnDeviceRemovedEvent().Subscribe([this](size_t listenerId, const std::shared_ptr<DBusDeviceInfo>& removedDeviceInfo) {
            LOG_DEBUG("OnDeviceRemoved: %s", removedDeviceInfo->GetAddress().c_str());
            
            if (_devicesMap.contains(removedDeviceInfo->GetAddress())) {
                _onDeviceRemoveEvent.FireEvent(_devicesMap.at(removedDeviceInfo->GetAddress()));
                _devicesMap.erase(removedDeviceInfo->GetAddress());
            }

            TrySelectNewActiveDevice();
        });

        _dbusService.IsBluetoothAdapterPowered().GetEvent().Subscribe([this](size_t listenerId, bool newPoweredValue) {
            _onDefaultAdapterChangeEnabled.FireEvent(newPoweredValue);
        });
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
        _dbusService.EnableBluetoothAdapter();
    }

    void DevicesInfoFetcher::EnableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _dbusService.EnableBluetoothAdapterAsync(std::move(callback));
    }

    void DevicesInfoFetcher::DisableBluetoothAdapter() {
        _dbusService.DisableBluetoothAdapter();
    }

    void DevicesInfoFetcher::DisableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback) {
        _dbusService.DisableBluetoothAdapterAsync(std::move(callback));
    }

    std::shared_ptr<Device> DevicesInfoFetcher::TryCreateDevice(const std::shared_ptr<DBusDeviceInfo>& deviceInfo) {
        /*
        // TODO: удалить?
        // HARDCODED TEST
        const static auto checkHardcodedModalias = [](const std::string& modalias) -> bool {
            LOG_RELEASE("%s", modalias.c_str());
            return modalias == "bluetooth:v0075pA013d0001";
        };
        // HARDCODED TEST
        */

        if (AapHelper::IsAapDevice(deviceInfo->GetProductId(), deviceInfo->GetVendorId())){
            auto newDevice = AapDevice::Create(deviceInfo);
            newDevice->GetConnectedPropertyChangedEvent().Subscribe([this](size_t listenerId, bool newValue) {
                TrySelectNewActiveDevice();
            });
            return newDevice;
        } 
        /*
        else if (deviceInterface.contains("Modalias") && checkHardcodedModalias(deviceInterface.at("Modalias").get<std::string>())) {
            //auto newDevice = GalaxyBudsDevice::Create(objectPath, deviceInterface, 9);
            //newDevice->GetConnectedPropertyChangedEvent().Subscribe([this](size_t listenerId, bool newValue) {
            //    TrySelectNewActiveDevice();
            //});
            //return newDevice;
        }
        */
        return nullptr;
    }

    void DevicesInfoFetcher::ClearAndFillDevicesMap() {
        _devicesMap.clear();
        _activeDevice = nullptr;

        for (const auto& deviceInfo : _dbusService.GetBtDevices()) {
            if (auto device = TryCreateDevice(deviceInfo)) {
                _devicesMap.emplace(deviceInfo->GetAddress(), device);
                _onDeviceAddEvent.FireEvent(device);
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
}
