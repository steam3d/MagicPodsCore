// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsBatteryWatcher.h"
#include "sdk/pbp/PixelBudsHelper.h"
#include "sdk/pbp/ProtoWire.h"
#include "device/enums/DeviceBatteryType.h"
#include "device/enums/DeviceBatteryStatus.h"

namespace MagicPodsCore
{
    namespace
    {
        // maestro_pw.RuntimeInfo
        constexpr unsigned int BatteryInfoField = 6;
        // maestro_pw.BatteryInfo
        constexpr unsigned int CaseBatteryField = 1;
        constexpr unsigned int LeftBatteryField = 2;
        constexpr unsigned int RightBatteryField = 3;
        // maestro_pw.DeviceBatteryInfo
        constexpr unsigned int LevelField = 1;
        constexpr unsigned int StateField = 2;
        // maestro_pw.BatteryState
        constexpr uint64_t BatteryCharging = 2;

        // The buds leave the whole sub-message out for a peer which is gone instead of sending a zero level, so a missing one is reported as disconnected rather than skipped
        DeviceBatteryData ExtractBattery(const ProtoField *field, DeviceBatteryType type)
        {
            DeviceBatteryData disconnected(type, DeviceBatteryStatus::Disconnected, 0, false);

            if (field == nullptr)
                return disconnected;

            std::optional<std::vector<ProtoField>> optionalFields = ProtoWire::Parse(field->Bytes);
            if (!optionalFields.has_value())
                return disconnected;

            const ProtoField *levelField = ProtoWire::Find(optionalFields.value(), LevelField, ProtoWireTypes::Varint);
            if (levelField == nullptr)
                return disconnected;

            const ProtoField *stateField = ProtoWire::Find(optionalFields.value(), StateField, ProtoWireTypes::Varint);
            bool isCharging = stateField != nullptr && stateField->Varint == BatteryCharging;
            short level = static_cast<short>(levelField->Varint);

            return DeviceBatteryData(
                type,
                DeviceBatteryStatus::Connected,
                level,
                isCharging);
        }
    }

    std::vector<DeviceBatteryData> PixelBudsBatteryWatcher::Process_SubscribeRuntimeInfo(const PwRpcPacket &packet)
    {
        std::vector<DeviceBatteryData> battery;

        std::optional<std::vector<ProtoField>> optionalRuntimeInfo = ProtoWire::Parse(packet.Payload);
        if (!optionalRuntimeInfo.has_value())
            return battery;

        const ProtoField *batteryInfoField = ProtoWire::Find(optionalRuntimeInfo.value(), BatteryInfoField, ProtoWireTypes::Bytes);
        if (batteryInfoField == nullptr)
            return battery;

        std::optional<std::vector<ProtoField>> optionalBatteryInfo = ProtoWire::Parse(batteryInfoField->Bytes);
        if (!optionalBatteryInfo.has_value())
            return battery;

        const auto &fields = optionalBatteryInfo.value();

        battery.push_back(ExtractBattery(ProtoWire::Find(fields, LeftBatteryField, ProtoWireTypes::Bytes), DeviceBatteryType::Left));
        battery.push_back(ExtractBattery(ProtoWire::Find(fields, RightBatteryField, ProtoWireTypes::Bytes), DeviceBatteryType::Right));
        battery.push_back(ExtractBattery(ProtoWire::Find(fields, CaseBatteryField, ProtoWireTypes::Bytes), DeviceBatteryType::Case));

        return battery;
    }

    void PixelBudsBatteryWatcher::ProcessResponse(const PwRpcPacket &packet)
    {
        if (packet.ServiceId != PixelBudsHelper::MaestroServiceId)
            return;

        if (packet.MethodId != PixelBudsHelper::SubscribeRuntimeInfoMethodId)
            return;

        if (packet.Type != PwRpcPacketTypes::ServerStream && packet.Type != PwRpcPacketTypes::Response)
            return;

        std::vector<DeviceBatteryData> battery = Process_SubscribeRuntimeInfo(packet);
        if (battery.size() > 0)
        {
            _batteryChanged.FireEvent(battery);
        }
    }
}
