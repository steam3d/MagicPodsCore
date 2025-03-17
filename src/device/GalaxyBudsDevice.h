#pragma once

#include "../Event.h"
#include "../Device.h"
#include "../sgb/GalaxyBudsHelper.h"
#include "../sgb/setters/GalaxyBudsSetAnc.h"
#include "../sgb/enums/GalaxyBudsModelIds.h"
#include "../sgb/structs/GalaxyBudsResponseData.h"

namespace MagicPodsCore
{
    class GalaxyBudsDevice : public Device
    {
    private:
        unsigned short _customProductId = 0;
        Event<GalaxyBudsResponseData> _onGalaxyBudsResponseDataRecived{};
        //Add client

    public:
        explicit GalaxyBudsDevice(const sdbus::ObjectPath &objectPath,
                                  const std::map<std::string, sdbus::Variant> &deviceInterface,                                  
                                  unsigned short model);

        unsigned short GetProductId() const override
        {
            std::lock_guard lock{_propertyMutex};
            return _customProductId;
        }

        Event<GalaxyBudsResponseData>& GetGalaxyBudsResponseDataRecived() {
            return _onGalaxyBudsResponseDataRecived;
        }   

        void SendData(const GalaxyBudsSetAnc& setter);
    };
}