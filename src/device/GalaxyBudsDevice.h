// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "Device.h"
#include "sdk/sgb/setters/GalaxyBudsSetAnc.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
#include "sdk/sgb/GalaxyBudsPacket.h"
#include "settings/SettingsService.h"

namespace MagicPodsCore
{
    class GalaxyBudsDevice : public Device
    {
    private:
        unsigned short _customProductId = 0;
        Event<GalaxyBudsResponseData> _ResponseDataRecived{};
        GalaxyBudsPacket _packet;
        void OnResponseDataReceived(const std::vector<unsigned char> &data) override;
    public:
        explicit GalaxyBudsDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, unsigned short model);

        unsigned short GetProductId() const override
        {
            std::lock_guard lock{_propertyMutex};
            return _customProductId;
        }

        Event<GalaxyBudsResponseData> &GetResponseDataRecived()
        {
            return _ResponseDataRecived;
        }

        void SendData(const GalaxyBudsSetAnc &setter);

        static std::unique_ptr<GalaxyBudsDevice> Create(std::shared_ptr<DBusDeviceInfo> deviceInfo,std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, unsigned short model);        
    };
}