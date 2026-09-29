// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsCapability.h"
#include "sdk/pbp/enums/PixelBudsModelIds.h"

namespace MagicPodsCore
{
    void PixelBudsCapability::SendData(const PixelBudsSetAnc &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        device.SendRpc(setter.MethodId, setter.Payload);
    }

    void PixelBudsCapability::Reset()
    {
        Capability::Reset();
    }

    PixelBudsCapability::PixelBudsCapability(const std::string &name,
                                             bool isReadOnly,
                                             PixelBudsDevice& device) : Capability(name, isReadOnly),
                                                                        device(device)
    {
        responseDataRecivedId = this->device.GetResponseDataRecived().Subscribe([this](size_t id, const PwRpcPacket &packet)
            {
                OnReceivedData(packet);
            });

        onClientStateChangedId = this->device.GetClientStateChangedEvent().Subscribe([this](size_t id, ClientState state)
            {
                if (state == ClientState::Disconnected) {
                    Reset();
                    _onChanged.FireEvent(*this);
                }
            });

    }

    PixelBudsCapability::~PixelBudsCapability()
    {
        //Do not use the device here. All events will be released in ~Device.
    }
}
