// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsDevice.h"

#include "sdk/sgb/GalaxyBudsHelper.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "capabilities/sgb/GalaxyBudsAncCapability.h"
#include "capabilities/sgb/GalaxyBudsBatteryCapability.h"
#include "capabilities/sgb/GalaxyBudsEarDetectionCapability.h"
#include "capabilities/cmn/CmnBluetoothCodecCapability.h"

namespace MagicPodsCore
{
    void GalaxyBudsDevice::OnResponseDataReceived(const std::vector<unsigned char> &data)
    {
        std::optional<GalaxyBudsResponseData> optionalData = _packet.Extract(data);
        if (optionalData.has_value())
            _ResponseDataRecived.FireEvent(optionalData.value());
    }

    GalaxyBudsDevice::GalaxyBudsDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, std::shared_ptr<MediaController> mediaController, unsigned short model)
        : Device(deviceInfo, audioClient, settingsService, mediaController),
          _customProductId(model),
          _packet(static_cast<GalaxyBudsModelIds>(model)) {}

    void GalaxyBudsDevice::SendData(const GalaxyBudsSetAnc &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        _client->SendData(_packet.Encode(setter.Id, setter.Payload));
    }

    std::shared_ptr<GalaxyBudsDevice> GalaxyBudsDevice::Create(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, std::shared_ptr<MediaController> mediaController, unsigned short model)
    {
        // Shared from the start, so Init() can already hand a weak reference to the deferred client restart.
        auto device = std::make_shared<GalaxyBudsDevice>(deviceInfo, audioClient, settingsService, mediaController, model);

        device->capabilities.push_back(std::make_unique<CmnBluetoothCodecCapability>(*device));
        device->capabilities.push_back(std::make_unique<GalaxyBudsBatteryCapability>(*device));
        device->capabilities.push_back(std::make_unique<GalaxyBudsAncCapability>(*device));
        
        //Ear Detection works out of the box, but it could be useful later with additional features based on it.
        //device->capabilities.push_back(std::make_unique<GalaxyBudsEarDetectionCapability>(*device));
        
        device->_client = Client::CreateRFCOMM(deviceInfo->GetAddress(), GalaxyBudsHelper::GetServiceGuid(static_cast<GalaxyBudsModelIds>(model)));
        device->Init();
        return device;
    }
}
