// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/capabilities/Capability.h"
#include "device/GalaxyBudsDevice.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
#include "sdk/sgb/setters/GalaxyBudsSetAnc.h"

namespace MagicPodsCore
{

    class GalaxyBudsCapability : public Capability
    {
    private:
        size_t responseDataRecivedId;
        size_t onConnectedPropertyChangedId;
    
    protected:
        GalaxyBudsDevice& device;
        virtual void OnReceivedData(const GalaxyBudsResponseData &data) = 0;
        void SendData(const GalaxyBudsSetAnc& setter);
        void Reset() override;

    public:
        explicit GalaxyBudsCapability(const std::string &name,
                                      bool isReadOnly,
                                      GalaxyBudsDevice& device);
        ~GalaxyBudsCapability() override;
    };
}