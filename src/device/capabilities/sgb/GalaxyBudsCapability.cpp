#include "GalaxyBudsCapability.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"

namespace MagicPodsCore
{
    void GalaxyBudsCapability::SendData(const GalaxyBudsSetAnc &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        device.SendData(setter);
    }

    void GalaxyBudsCapability::Reset()
    {
        Capability::Reset();
    }

    GalaxyBudsCapability::GalaxyBudsCapability(const std::string &name,
                                               bool isReadOnly,
                                               GalaxyBudsDevice& device) : Capability(name, isReadOnly),
                                                                           device(device)
    {
        responseDataRecivedId = this->device.GetResponseDataRecived().Subscribe([this](size_t id, const GalaxyBudsResponseData &data)
            {
                OnReceivedData(data);
            });

        onConnectedPropertyChangedId = this->device.GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, bool isConnected)
            {
                if (!isConnected)
                    Reset();
            });

    }

    GalaxyBudsCapability::~GalaxyBudsCapability()
    {
        //Do not use the device here. All events will be released in ~Device.
    }
}