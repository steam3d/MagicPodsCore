// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsAncWatcher.h"
#include "sdk/pbp/PixelBudsHelper.h"
#include "sdk/pbp/ProtoWire.h"

namespace MagicPodsCore
{

    std::optional<PixelBudsAncState> PixelBudsAncWatcher::ExtractState(const std::vector<unsigned char> &settingsRsp)
    {
        if (!IsSupport(model))
            return std::nullopt;

        std::optional<std::vector<ProtoField>> optionalResponse = ProtoWire::Parse(settingsRsp);
        if (!optionalResponse.has_value())
            return std::nullopt;

        const ProtoField *valueField = ProtoWire::Find(optionalResponse.value(), PixelBudsHelper::SettingsRspValueField, ProtoWireTypes::Bytes);
        if (valueField == nullptr)
            return std::nullopt;

        std::optional<std::vector<ProtoField>> optionalValue = ProtoWire::Parse(valueField->Bytes);
        if (!optionalValue.has_value())
            return std::nullopt;

        const ProtoField *stateField = ProtoWire::Find(optionalValue.value(), PixelBudsHelper::CurrentAncrStateValueField, ProtoWireTypes::Varint);
        if (stateField == nullptr)
            return std::nullopt;

        unsigned char state = static_cast<unsigned char>(stateField->Varint & 0xFF);
        if (!isValidPixelBudsAncStateType(state))
            return std::nullopt;

        return static_cast<PixelBudsAncState>(state);
    }

    void PixelBudsAncWatcher::ProcessResponse(const PwRpcPacket &packet)
    {
        if (packet.ServiceId != PixelBudsHelper::MaestroServiceId)
            return;

        std::optional<PixelBudsAncState> stateOpt;

        if (packet.MethodId == PixelBudsHelper::ReadSettingMethodId)
        {
            if (packet.Type == PwRpcPacketTypes::Response)
                stateOpt = ExtractState(packet.Payload);
        }
        else if (packet.MethodId == PixelBudsHelper::SubscribeToSettingsChangesMethodId)
        {
            if (packet.Type == PwRpcPacketTypes::ServerStream || packet.Type == PwRpcPacketTypes::Response)
                stateOpt = ExtractState(packet.Payload);
        }

        if (stateOpt.has_value())
        {
            PixelBudsAncState state = stateOpt.value();
            _ancChanged.FireEvent(state);
        }
    }

    bool PixelBudsAncWatcher::IsSupport(PixelBudsModelIds model)
    {
        return (model == PixelBudsModelIds::PixelBudsPro ||
                model == PixelBudsModelIds::PixelBudsPro2);
    }

    std::vector<PixelBudsAncState> PixelBudsAncWatcher::GetAncModesFor(PixelBudsModelIds model)
    {
        if (model == PixelBudsModelIds::PixelBudsPro)
        {
            return std::vector<PixelBudsAncState>{
                PixelBudsAncState::Off,
                PixelBudsAncState::Aware,
                PixelBudsAncState::Active,
            };
        }
        else if (model == PixelBudsModelIds::PixelBudsPro2)
        {
            return std::vector<PixelBudsAncState>{
                PixelBudsAncState::Off,
                PixelBudsAncState::Aware,
                PixelBudsAncState::Adaptive,
                PixelBudsAncState::Active,
            };
        }
        else
        {
            return std::vector<PixelBudsAncState>{};
        }
    }
}
