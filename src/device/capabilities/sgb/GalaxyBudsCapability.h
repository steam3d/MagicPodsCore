#pragma once

#include "../Capability.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
#include "sdk/sgb/setters/GalaxyBudsSetAnc.h"
#include "../../GalaxyBudsDevice.h"

namespace MagicPodsCore
{

    class GalaxyBudsCapability : public Capability
    {
    private:
        size_t responseDataRecivedId;
    
    protected:
        GalaxyBudsDevice &device;
        virtual void OnReceivedData(const GalaxyBudsResponseData &data) = 0;
        void SendData(const GalaxyBudsSetAnc& setter);

    public:
        explicit GalaxyBudsCapability(const std::string &name,
                                      bool isReadOnly,
                                      GalaxyBudsDevice &device);
        ~GalaxyBudsCapability() override;
    };
}