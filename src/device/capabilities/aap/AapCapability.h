// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

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
        AapDevice& device;
        virtual void OnReceivedData(const std::vector<unsigned char> &data) = 0;
        void SendData(const AapRequest &setter);
        void Reset() override;


    public:
        explicit AapCapability(const std::string &name,
                               bool isReadOnly,
                               AapDevice& device);
        ~AapCapability() override;
    };
}