// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "BhfDevice.h"
#include "capabilities/bhf/BhfBatteryCapability.h"

namespace MagicPodsCore
{
    void BhfDevice::OnResponseDataReceived(const std::vector<unsigned char> &data){}

    BhfDevice::BhfDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo) : Device(deviceInfo)
    {
    }

    std::unique_ptr<BhfDevice> BhfDevice::Create(std::shared_ptr<DBusDeviceInfo> deviceInfo)
    {
        auto device = std::make_unique<BhfDevice>(deviceInfo);

        device->capabilities.push_back(std::make_unique<BhfBatteryCapability>(*device));
        device->Init();
        return device;
    }
}