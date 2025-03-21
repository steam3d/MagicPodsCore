#include "AapCapability.h"

namespace MagicPodsCore
{
    void AapCapability::SendData(const AapRequest &setter)
    {
        device.SendData(setter);
    }

    void AapCapability::Reset()
    {
        Capability::Reset();
    }

    AapCapability::AapCapability(const std::string &name,
                                 bool isReadOnly,
                                 AapDevice &device) : Capability(name, isReadOnly),
                                                      device(device)
    {
        responseDataRecivedId = device.GetResponseDataRecived().Subscribe([this](size_t id, const std::vector<unsigned char> &data)
            {
                OnReceivedData(data);
            });

        onConnectedPropertyChangedId = device.GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, bool isConnected)
            {
                if (!isConnected)
                    Reset();
            });
    }

    AapCapability::~AapCapability()
    {
        device.GetResponseDataRecived().Unsubscribe(responseDataRecivedId);
        device.GetResponseDataRecived().Unsubscribe(onConnectedPropertyChangedId);
    }
}