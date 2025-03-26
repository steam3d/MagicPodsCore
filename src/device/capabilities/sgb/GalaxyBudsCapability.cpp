#include "GalaxyBudsCapability.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"

namespace MagicPodsCore
{
    void GalaxyBudsCapability::SendData(const GalaxyBudsSetAnc &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        if (auto dev = this->weakDevice.lock()){
            dev->SendData(setter);
        }
    }

    void GalaxyBudsCapability::Reset()
    {
        Capability::Reset();
    }

    GalaxyBudsCapability::GalaxyBudsCapability(const std::string &name,
                                               bool isReadOnly,
                                               std::shared_ptr<GalaxyBudsDevice> device) : Capability(name, isReadOnly),
                                                                           weakDevice(device)
    {
        if (auto dev = this->weakDevice.lock()) {
            responseDataRecivedId = dev->GetResponseDataRecived().Subscribe([this](size_t id, const GalaxyBudsResponseData &data)
                {
                    OnReceivedData(data);
                });

            onConnectedPropertyChangedId = dev->GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, bool isConnected)
                {
                    if (!isConnected)
                        Reset();
                });
        }
    }

    GalaxyBudsCapability::~GalaxyBudsCapability()
    {
        //Do not use the device here. All events will be released in ~Device.
    }
}