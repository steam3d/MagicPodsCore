#pragma once

#include "Event.h"
#include "Device.h"
#include "sdk/sgb/setters/GalaxyBudsSetAnc.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
#include "sdk/sgb/GalaxyBudsPacket.h"

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
        explicit GalaxyBudsDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo, unsigned short model);

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

        static std::unique_ptr<GalaxyBudsDevice> Create(std::shared_ptr<DBusDeviceInfo> deviceInfo, unsigned short model);        
    };
}