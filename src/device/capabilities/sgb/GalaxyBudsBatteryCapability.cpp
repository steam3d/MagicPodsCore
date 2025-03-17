#include "GalaxyBudsBatteryCapability.h"

namespace MagicPodsCore
{

    GalaxyBudsBatteryCapability::GalaxyBudsBatteryCapability(GalaxyBudsDevice &device) : GalaxyBudsCapability("battery", true, device),
                                                                                         battery(true),
                                                                                         watcher(GalaxyBudsBatteryWatcher(static_cast<GalaxyBudsModelIds>(device.GetProductId())))
    {
        this->GetBatteryChangedEventId = this->watcher.GetBatteryChangedEvent().Subscribe([this](size_t id, std::vector<DeviceBatteryData> b)
                                                         {
            if (!isAvailable)
                isAvailable = true;

            battery.UpdateBattery(b); });
    }

    nlohmann::json GalaxyBudsBatteryCapability::CreateJsonBody()
    {
        return battery.CreateJsonBody();
    }

    void GalaxyBudsBatteryCapability::OnReceivedData(const GalaxyBudsResponseData &data)
    {
        watcher.ProcessResponse(data);
    }

    GalaxyBudsBatteryCapability::~GalaxyBudsBatteryCapability(){
        watcher.GetBatteryChangedEvent().Unsubscribe(GetBatteryChangedEventId);
    }

}