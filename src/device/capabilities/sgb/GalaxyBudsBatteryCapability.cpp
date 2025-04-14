// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsBatteryCapability.h"

namespace MagicPodsCore
{
    nlohmann::json GalaxyBudsBatteryCapability::CreateJsonBody()
    {
        return battery.CreateJsonBody();
    }

    void GalaxyBudsBatteryCapability::OnReceivedData(const GalaxyBudsResponseData &data)
    {
        watcher.ProcessResponse(data);
    }

    void GalaxyBudsBatteryCapability::Reset()
    {
        battery.ClearBattery();
        GalaxyBudsCapability::Reset();
    }

    GalaxyBudsBatteryCapability::GalaxyBudsBatteryCapability(GalaxyBudsDevice& device) : GalaxyBudsCapability("battery", true, device),
                                                                                         battery(true),
                                                                                         watcher(GalaxyBudsBatteryWatcher(static_cast<GalaxyBudsModelIds>(device.GetProductId())))
    {
        batteryChangedEventId = battery.GetBatteryChangedEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            if (!isAvailable)
                isAvailable = true;

            _onChanged.FireEvent(*this);
            Logger::Debug("AapAncCapability::GetBatteryChangedEvent");
        });

        watcherBatteryChangedEventId = watcher.GetBatteryChangedEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            battery.UpdateBattery(b); });
    }

    GalaxyBudsBatteryCapability::~GalaxyBudsBatteryCapability()
    {
        battery.GetBatteryChangedEvent().Unsubscribe(batteryChangedEventId);
        watcher.GetBatteryChangedEvent().Unsubscribe(watcherBatteryChangedEventId);
    }

}