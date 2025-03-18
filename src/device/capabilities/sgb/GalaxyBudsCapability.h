#pragma once

#include "../Capability.h"
#include "../../sgb/enums/GalaxyBudsModelIds.h"
#include "../../sgb/structs/GalaxyBudsResponseData.h"
#include "../../sgb/setters/GalaxyBudsSetAnc.h"
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