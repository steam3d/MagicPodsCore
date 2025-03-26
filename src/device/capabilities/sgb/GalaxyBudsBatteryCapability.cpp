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

    GalaxyBudsBatteryCapability::GalaxyBudsBatteryCapability(std::shared_ptr<GalaxyBudsDevice> device) : GalaxyBudsCapability("battery", true, device),
                                                                                         battery(true),
                                                                                         watcher(GalaxyBudsBatteryWatcher(static_cast<GalaxyBudsModelIds>(device->GetProductId())))
    {
        batteryChangedEventId = battery.GetBatteryChangedEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            if (!isAvailable)
                isAvailable = true;

            _onChanged.FireEvent(*this);
            LOG_DEBUG("AapAncCapability::GetBatteryChangedEvent");
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