// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "BhfBatteryCapability.h"
#include "Logger.h"


namespace MagicPodsCore
{

    nlohmann::json BhfBatteryCapability::CreateJsonBody()
    {
        return battery.CreateJsonBody();
    }

    void BhfBatteryCapability::Reset()
    {
        battery.ClearBattery();
        Capability::Reset();
    }

    void BhfBatteryCapability::UpdateSingleBattery(uint8_t level)
    {
        std::vector<DeviceBatteryData> deviceBattery;
        DeviceBatteryData single(
            DeviceBatteryType::Single,
            DeviceBatteryStatus::Connected,
            level,
            false);
        deviceBattery.push_back(single);
        battery.UpdateBattery(deviceBattery);
    }

    BhfBatteryCapability::BhfBatteryCapability(BhfDevice& device) : Capability("battery", true),
                                                                    device(device),
                                                                    battery(true)
    {

        onHandsFreeBatteryPropertyChangedId = device.GetHandsFreeBatteryPropertyChangedEvent().Subscribe([this](size_t id, const uint8_t level){
            UpdateSingleBattery(level);
        });

        onConnectedPropertyChangedId = device.GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, const bool b){
            if (!b) {
                Reset();
            }
        });

        batteryChangedEventId = battery.GetBatteryChangedEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            if (!isAvailable)
                isAvailable = true;

            _onChanged.FireEvent(*this);
        });

        if (device.GetConnected()){
            UpdateSingleBattery(device.GetHandsFreeBattery());
        }
    }

    BhfBatteryCapability::~BhfBatteryCapability()
    {
        battery.GetBatteryChangedEvent().Unsubscribe(batteryChangedEventId);
        device.GetConnectedPropertyChangedEvent().Unsubscribe(onConnectedPropertyChangedId);
        device.GetHandsFreeBatteryPropertyChangedEvent().Unsubscribe(onHandsFreeBatteryPropertyChangedId);
    }
}
