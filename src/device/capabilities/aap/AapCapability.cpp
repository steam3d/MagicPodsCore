#include "AapCapability.h"

namespace MagicPodsCore
{
    void AapCapability::SendData(const AapRequest &setter)
    {
        if (auto dev = this->weakDevice.lock()) {
            dev->SendData(setter);
        }
    }

    void AapCapability::Reset()
    {
        Capability::Reset();
    }

    AapCapability::AapCapability(const std::string &name,
                                 bool isReadOnly,
                                 std::shared_ptr<AapDevice> device) : Capability(name, isReadOnly),
                                                      weakDevice(device)
    {
        if (auto dev = this->weakDevice.lock()) {
            responseDataRecivedId = dev->GetResponseDataRecived().Subscribe([this](size_t id, const std::vector<unsigned char> &data)
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

    AapCapability::~AapCapability()
    {
        //Do not use the device here. All events will be released in ~Device.
    }
}