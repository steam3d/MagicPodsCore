#pragma once

#include "device/capabilities/Capability.h"
#include "sdk/aap/setters/AapRequest.h"
#include "device/AapDevice.h"

namespace MagicPodsCore
{
    class AapCapability : public Capability
    {
    private:
        size_t responseDataRecivedId;
        size_t onConnectedPropertyChangedId;

    protected:
        std::weak_ptr<AapDevice> weakDevice;
        virtual void OnReceivedData(const std::vector<unsigned char> &data) = 0;
        void SendData(const AapRequest &setter);
        void Reset() override;


    public:
        explicit AapCapability(const std::string &name,
                               bool isReadOnly,
                               std::shared_ptr<AapDevice> device);
        ~AapCapability() override;
    };
}