#include "GalaxyBudsCapability.h"

namespace MagicPodsCore
{
    GalaxyBudsCapability::GalaxyBudsCapability(const std::string &name,
                                               bool isReadOnly,
                                               GalaxyBudsDevice &device) : Capability(name, isReadOnly),
                                                                           device(device)
    {
        this->GalaxyBudsResponseDataRecivedId = this->device.GetGalaxyBudsResponseDataRecived().Subscribe([this](size_t id, const GalaxyBudsResponseData &data)
                                                                             { this->OnReceivedData(data); });
    }

    void GalaxyBudsCapability::SendData(const GalaxyBudsSetAnc& setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        device.SendData(setter);
    }

    GalaxyBudsCapability::~GalaxyBudsCapability()
    {
        device.GetGalaxyBudsResponseDataRecived().Unsubscribe(GalaxyBudsResponseDataRecivedId);
    }
}