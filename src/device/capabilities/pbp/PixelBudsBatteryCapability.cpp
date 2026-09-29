// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsBatteryCapability.h"

namespace MagicPodsCore
{
    nlohmann::json PixelBudsBatteryCapability::CreateJsonBody()
    {
        return battery.CreateJsonBody();
    }

    void PixelBudsBatteryCapability::OnReceivedData(const PwRpcPacket &packet)
    {
        watcher.ProcessResponse(packet);
    }

    void PixelBudsBatteryCapability::Reset()
    {
        battery.ClearBattery();
        PixelBudsCapability::Reset();
    }

    PixelBudsBatteryCapability::PixelBudsBatteryCapability(PixelBudsDevice& device) : PixelBudsCapability("battery", true, device),
                                                                                      battery(true),
                                                                                      watcher(PixelBudsBatteryWatcher(static_cast<PixelBudsModelIds>(device.GetProductId())))
    {
        batteryChangedEventId = battery.GetBatteryChangedEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            if (!isAvailable)
                isAvailable = true;

            _onChanged.FireEvent(*this);
            Logger::Debug("PixelBudsBatteryCapability::GetBatteryChangedEvent");
        });

        watcherBatteryChangedEventId = watcher.GetBatteryChangedEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            battery.UpdateBattery(b); });
    }

    PixelBudsBatteryCapability::~PixelBudsBatteryCapability()
    {
        battery.GetBatteryChangedEvent().Unsubscribe(batteryChangedEventId);
        watcher.GetBatteryChangedEvent().Unsubscribe(watcherBatteryChangedEventId);
    }

}
