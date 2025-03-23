#pragma once

#include "../Capability.h"
#include "../../../aap/setters/AapRequest.h"
#include "../../AapDevice.h"
//class AapDevice;

namespace MagicPodsCore
{
    class AapCapability : public Capability
    {
    private:
        size_t responseDataRecivedId;
        size_t onConnectedPropertyChangedId;

    protected:
        AapDevice &device;
        virtual void OnReceivedData(const std::vector<unsigned char> &data) = 0;
        void SendData(const AapRequest &setter);
        void Reset() override;


    public:
        explicit AapCapability(const std::string &name,
                               bool isReadOnly,
                               AapDevice &device);
        ~AapCapability() override;
    };
}