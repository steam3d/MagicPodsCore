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
        Event<GalaxyBudsResponseData> _onGalaxyBudsResponseDataRecived{};
        GalaxyBudsPacket _packet;
        void OnResponseDataReceived(const std::vector<unsigned char> &data) override;

        explicit GalaxyBudsDevice(const sdbus::ObjectPath &objectPath,
                                  const std::map<std::string, sdbus::Variant> &deviceInterface,
                                  unsigned short model);

    public:
        unsigned short GetProductId() const override
        {
            std::lock_guard lock{_propertyMutex};
            return _customProductId;
        }

        Event<GalaxyBudsResponseData> &GetGalaxyBudsResponseDataRecived()
        {
            return _onGalaxyBudsResponseDataRecived;
        }

        void SendData(const GalaxyBudsSetAnc &setter);

        static std::unique_ptr<GalaxyBudsDevice> Create(const sdbus::ObjectPath &objectPath,
                                                        const std::map<std::string, sdbus::Variant> &deviceInterface,
                                                        unsigned short model);
    };
}