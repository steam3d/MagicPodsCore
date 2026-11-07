// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsDevice.h"

#include "sdk/sgb/GalaxyBudsHelper.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "capabilities/sgb/GalaxyBudsAncCapability.h"
#include "capabilities/sgb/GalaxyBudsBatteryCapability.h"
#include "capabilities/sgb/GalaxyBudsEarDetectionCapability.h"
#include "capabilities/sgb/GalaxyBudsEarDetectionStateCapability.h"
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
        //device->capabilities.push_back(std::make_unique<GalaxyBudsEarDetectionStateCapability>(*device));
        
        device->_client = Client::CreateRFCOMM(deviceInfo->GetAddress(), GalaxyBudsHelper::GetServiceGuid(static_cast<GalaxyBudsModelIds>(model)));
        device->Init();

        // Auto Switch. The second link (scn 29) is what carries the device group table and the
        // focus/class sub-messages; the policy only acts while the "auto_switch.enabled" setting
        // is on, which it is not by default.
        device->_smepChannel = std::make_shared<SmepChannel>(deviceInfo->GetAddress());
        // The PipeWire watcher behind the takeover path: it answers "did OUR audio just go
        // onto the buds", which is the event the claim has to beat the buds' own pause to.
        device->_audioStreamWatch = std::make_shared<AudioStreamWatch>(deviceInfo->GetAddress());
        // The A4 cradle gate needs to know whether the device's own control link is up: a
        // connected device proves the buds are awake, which stops the (link-fed) closed-cradle
        // cache from gating the SMEP link out forever (17.21).
        std::weak_ptr<GalaxyBudsDevice> selfWeak = device;
        device->_autoSwitch = std::make_unique<AutoSwitchService>(device->_smepChannel, device->_audioStreamWatch,
                                                                  mediaController, settingsService,
                                                                  [selfWeak]()
                                                                  {
                                                                      const auto self = selfWeak.lock();
                                                                      return self && self->GetConnected();
                                                                  });
        device->_autoSwitch->Start();
        return device;
    }
}
