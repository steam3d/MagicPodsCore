// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapDevice.h"
#include "capabilities/aap/AapAncCapability.h"
#include "capabilities/aap/AapConversationAwarenessCapability.h"
#include "capabilities/aap/AapConversationAwarenessStateCapability.h"
#include "capabilities/aap/AapNoiseCancellationOneAirPodModeCapability.h"
#include "capabilities/aap/AapPressAndHoldDurationCapability.h"
#include "capabilities/aap/AapPressSpeedCapability.h"
#include "capabilities/aap/AapVolumeSwipeCapability.h"
#include "capabilities/aap/AapVolumeSwipeLengthCapability.h"
#include "capabilities/aap/AapPersonalizedVolumeCapability.h"
#include "capabilities/aap/AapToneVolumeCapability.h"
#include "capabilities/aap/AapMuteMicrophoneEndCallCapability.h"
#include "capabilities/aap/AapAdaptiveAudioNoiseCapability.h"
#include "capabilities/aap/AapBatteryCapability.h"
#include "sdk/aap/setters/AapInit.h"
#include "sdk/aap/setters/AapInitExt.h"
#include "sdk/aap/setters/AapEnableNotifications.h"

namespace MagicPodsCore
{
    void AapDevice::OnResponseDataReceived(const std::vector<unsigned char> &data)
    {
        _onResponseDataRecived.FireEvent(data);
    }

    AapDevice::AapDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo) : Device(deviceInfo)
    {
    }

    void AapDevice::SendData(const AapRequest &setter) //TODO: MAKE COMMON CLASS FOR SETTERS
    {
        _client->SendData(setter.Request());
    }

    std::unique_ptr<AapDevice> AapDevice::Create(std::shared_ptr<DBusDeviceInfo> deviceInfo)
    {
        auto device = std::make_unique<AapDevice>(deviceInfo);

        device->capabilities.push_back(std::make_unique<AapBatteryCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapAncCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapConversationAwarenessCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapConversationAwarenessStateCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapNoiseCancellationOneAirPodModeCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapPressAndHoldDurationCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapPressSpeedCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapVolumeSwipeCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapVolumeSwipeLengthCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapPersonalizedVolumeCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapToneVolumeCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapMuteMicrophoneEndCallCapability>(*device));
        device->capabilities.push_back(std::make_unique<AapAdaptiveAudioNoiseCapability>(*device));

        device->_clientStartData.push_back(AapInit{}.Request());
        device->_clientStartData.push_back(AapEnableNotifications{AapNotificationsMode::Unknown2}.Request());
        device->_clientStartData.push_back(AapEnableNotifications{AapNotificationsMode::Unknown1}.Request());
        if (AapInitExt::IsSupported(deviceInfo->GetProductId()))
            device->_clientStartData.push_back(AapInitExt{}.Request());

        //TODO: Add initData to client
        device->_client = Client::CreateL2CAP(deviceInfo->GetAddress(), 0x1001);

        device->Init();
        return device;
    }
}