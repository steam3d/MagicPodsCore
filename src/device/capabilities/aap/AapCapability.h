#pragma once

#include "../Capability.h"
#include "../../../aap/setters/AapRequest.h"
#include "../../AapDevice.h"
class AapDevice;

namespace MagicPodsCore
{
    class AapCapability : public Capability
    {
    private:
        size_t responseDataRecivedId;

    protected:    
        AapDevice &device;
        virtual void OnReceivedData(const std::vector<unsigned char> &data) = 0;
        void SendData(const AapRequest &setter);

    public:
        explicit AapCapability(const std::string &name,
                               bool isReadOnly,
                               AapDevice &device);
        ~AapCapability() override;
    };
}