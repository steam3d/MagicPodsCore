#include "AapCapability.h"

namespace MagicPodsCore
{
    void AapCapability::SendData(const AapRequest &setter)
    {
        device.SendData(setter);
    }

    AapCapability::AapCapability(const std::string &name,
                                 bool isReadOnly,
                                 AapDevice &device) : Capability(name, isReadOnly),
                                                      device(device)
    {
        responseDataRecivedId = device.GetResponseDataRecived().Subscribe([this](size_t id, const std::vector<unsigned char> &data)
                                                                          { OnReceivedData(data); });
    }

    AapCapability::~AapCapability()
    {
        device.GetResponseDataRecived().Unsubscribe(responseDataRecivedId);
    }
}