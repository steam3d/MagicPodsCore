// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "BhfDevice.h"
#include "capabilities/bhf/BhfBatteryCapability.h"
#include "capabilities/cmn/CmnBluetoothCodecCapability.h"

namespace MagicPodsCore
{
    void BhfDevice::OnResponseDataReceived(const std::vector<unsigned char> &data){}

    BhfDevice::BhfDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService) : Device(deviceInfo, audioClient, settingsService)
    {
    }

    std::unique_ptr<BhfDevice> BhfDevice::Create(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService)
    {
        auto device = std::make_unique<BhfDevice>(deviceInfo, audioClient, settingsService);

        device->capabilities.push_back(std::make_unique<BhfBatteryCapability>(*device));
        device->capabilities.push_back(std::make_unique<CmnBluetoothCodecCapability>(*device));
        device->Init();
        return device;
    }
}