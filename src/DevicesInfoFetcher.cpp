// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "DevicesInfoFetcher.h"

#include "BtVendorIds.h"
#include "sdk/aap/AapHelper.h"
#include "sdk/sgb/GalaxyBudsHelper.h"
#include "StringUtils.h"
#include "Logger.h"
#include "device/GalaxyBudsDevice.h"
#include "device/AapDevice.h"
#include "device/BhfDevice.h"

#include <regex>
#include <iostream>
#include <algorithm>

namespace MagicPodsCore {

    void DevicesInfoFetcher::UpdateBleState()
    {
        toml::v3::node_view<toml::v3::node> settingValue = _settingsService->GetSetting("magicpods", "animation");        
        bool value = true; // default setting

        if (settingValue.is_boolean())
            value = settingValue.as_boolean()->get();        
        
        if (value){
            _bleService->StartListening();
            _bleService->StartScan(true);
            Logger::Debug("Ble service started");
        }
        else
        {
            _bleService->StopScan();
            Logger::Debug("Ble service stopped");
        }
    }

    DevicesInfoFetcher::DevicesInfoFetcher(const std::shared_ptr<SettingsService> &settingsService): _settingsService{settingsService} {
        _audioClient = std::make_shared<PulseAudioClient>();
        _bleService = std::make_shared<DBusBasedBleAdvertisingService>(_dbusService);
        _onSettingsChangeId = _settingsService->GetOnSettingUpdateEvent().Subscribe([this](size_t id, const UpdatedSettingNotification& notification){
        if (notification.GetContainerName() == "magicpods" && notification.GetSettingName() == "animation")
                UpdateBleState();            
        });
                
        UpdateBleState();
        
        ClearAndFillDevicesMap();

        for (auto& device : _dbusService.GetAllDevices()) {
            Logger::Info("Device with address (known): %s", device->GetAddress().c_str());
        }

        _dbusService.GetOnDeviceAddedEvent().Subscribe([this](size_t listenerId, const std::shared_ptr<DBusDeviceInfo>& addedDeviceInfo) {
            Logger::Debug("OnDeviceAdded: %s", addedDeviceInfo->GetAddress().c_str());
            Logger::Info("Device with address (add): %s", addedDeviceInfo->GetAddress().c_str());
            if (!_devicesMap.contains(addedDeviceInfo->GetAddress())) {
                if (auto device = TryCreateDevice(addedDeviceInfo)) {
                    _devicesMap.emplace(addedDeviceInfo->GetAddress(), device);
                    _onDeviceAddEvent.FireEvent(device);
                }
                //TrySelectNewActiveDevice();
                // TODO: уведомление о добавлении устройства
            }
        });

        _dbusService.GetOnDeviceRemovedEvent().Subscribe([this](size_t listenerId, const std::shared_ptr<DBusDeviceInfo>& removedDeviceInfo) {
            Logger::Debug("OnDeviceRemoved: %s", removedDeviceInfo->GetAddress().c_str());
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

DevicesInfoFetcher::~DevicesInfoFetcher()
{
    _bleService->StopScan(); //wrong we must check if it is running
    _settingsService->GetOnSettingUpdateEvent().Unsubscribe(_onSettingsChangeId);
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
        Logger::Info("DevicesInfoFetcher::SetCapabilities");

        if (!json.contains("arguments") || !json["arguments"].contains("address") || !json["arguments"].contains("capabilities"))
        {
            Logger::Info("Error: missing required fields in SetCapabilities");
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

        Logger::Debug("%s (%s)", deviceInfo->GetName().c_str(), deviceInfo->GetAddress().c_str());
        Logger::Debug("    Vendor: %d",deviceInfo->GetVendorId());
        Logger::Debug("    Product: %d",deviceInfo->GetProductId());
        Logger::Debug("    Services:");
        for (const auto& uuid : deviceInfo->GetUuids()) {
            Logger::Debug("        %s", uuid.c_str());
        }

        if (AapHelper::IsAapDevice(deviceInfo->GetVendorId(), deviceInfo->GetProductId())){
            auto newDevice = AapDevice::Create(deviceInfo, _audioClient, _settingsService, _bleService);
            newDevice->GetConnectedPropertyChangedEvent().Subscribe([this](size_t listenerId, bool newValue) {
                TrySelectNewActiveDevice();
            });
            return newDevice;
        }

        else if (std::pair<GalaxyBudsModelIds, std::string> keyPair{};
                 GalaxyBudsHelper::IsGalaxyBudsDevice(deviceInfo->GetUuids()) &&
                 ((keyPair = GalaxyBudsHelper::SearchModelColor(deviceInfo->GetUuids(), deviceInfo->GetName())).first != GalaxyBudsModelIds::Unknown))
        {
            auto newDevice = GalaxyBudsDevice::Create(deviceInfo,_audioClient, _settingsService, static_cast<unsigned short>(keyPair.first));
            newDevice->GetConnectedPropertyChangedEvent().Subscribe([this](size_t listenerId, bool newValue) {
                TrySelectNewActiveDevice();
            });
            return newDevice;
        }
        //search headphones with handsfree service
        else if (auto uuids = deviceInfo->GetUuids();
                std::find(uuids.begin(), uuids.end(), "0000111e-0000-1000-8000-00805f9b34fb") != uuids.end()) {

                auto newDevice = BhfDevice::Create(deviceInfo,_audioClient, _settingsService);
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

        for (const auto& deviceInfo : _dbusService.GetPairedDevices()) {
            if (auto device = TryCreateDevice(deviceInfo)) {
                _devicesMap.emplace(deviceInfo->GetAddress(), device);
                _onDeviceAddEvent.FireEvent(device);
            }
        }

        TrySelectNewActiveDevice();

        Logger::Info("Devices created: %zu", _devicesMap.size());
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
