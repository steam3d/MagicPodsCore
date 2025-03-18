#include "GalaxyBudsCapability.h"

namespace MagicPodsCore
{
    void GalaxyBudsCapability::SendData(const GalaxyBudsSetAnc &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        device.SendData(setter);
    }

    GalaxyBudsCapability::GalaxyBudsCapability(const std::string &name,
                                               bool isReadOnly,
                                               GalaxyBudsDevice &device) : Capability(name, isReadOnly),
                                                                           device(device)
    {
        responseDataRecivedId = device.GetGalaxyBudsResponseDataRecived().Subscribe([this](size_t id, const GalaxyBudsResponseData &data)
                                                                                    { OnReceivedData(data); });
    }

    GalaxyBudsCapability::~GalaxyBudsCapability()
    {
        device.GetGalaxyBudsResponseDataRecived().Unsubscribe(responseDataRecivedId);
    }
}