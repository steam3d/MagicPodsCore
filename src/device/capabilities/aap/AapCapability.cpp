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
                                 AapDevice& device) : Capability(name, isReadOnly),
                                                      device(device)
    {
        responseDataRecivedId = this->device.GetResponseDataRecived().Subscribe([this](size_t id, const std::vector<unsigned char> &data)
            {
                OnReceivedData(data);
            });

        onConnectedPropertyChangedId = this->device.GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, bool isConnected)
            {
                if (!isConnected)
                    Reset();
            });
    }

    AapCapability::~AapCapability()
    {
        //Do not use the device here. All events will be released in ~Device.
    }
}