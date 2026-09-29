// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/capabilities/Capability.h"
#include "device/PixelBudsDevice.h"
#include "sdk/pbp/PwRpcPacket.h"
#include "sdk/pbp/setters/PixelBudsSetAnc.h"

namespace MagicPodsCore
{

    class PixelBudsCapability : public Capability
    {
    private:
        size_t responseDataRecivedId;
        size_t onClientStateChangedId;

    protected:
        PixelBudsDevice& device;
        virtual void OnReceivedData(const PwRpcPacket &packet) = 0;
        void SendData(const PixelBudsSetAnc& setter);
        void Reset() override;

    public:
        explicit PixelBudsCapability(const std::string &name,
                                     bool isReadOnly,
                                     PixelBudsDevice& device);
        ~PixelBudsCapability() override;
    };
}
