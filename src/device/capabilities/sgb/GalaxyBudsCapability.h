#pragma once

#include "device/capabilities/Capability.h"
#include "device/GalaxyBudsDevice.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
#include "sdk/sgb/setters/GalaxyBudsSetAnc.h"

namespace MagicPodsCore
{

    class GalaxyBudsCapability : public Capability
    {
    private:
        size_t responseDataRecivedId;
        size_t onConnectedPropertyChangedId;
    
    protected:
        std::weak_ptr<GalaxyBudsDevice> weakDevice;
        virtual void OnReceivedData(const GalaxyBudsResponseData &data) = 0;
        void SendData(const GalaxyBudsSetAnc& setter);
        void Reset() override;

    public:
        explicit GalaxyBudsCapability(const std::string &name,
                                      bool isReadOnly,
                                      std::shared_ptr<GalaxyBudsDevice> device);
        ~GalaxyBudsCapability() override;
    };
}