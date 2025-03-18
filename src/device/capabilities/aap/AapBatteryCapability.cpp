#include "AapBatteryCapability.h"

namespace MagicPodsCore
{

    nlohmann::json AapBatteryCapability::CreateJsonBody()
    {
        return battery.CreateJsonBody();
    }

    void AapBatteryCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapBatteryCapability::AapBatteryCapability(AapDevice &device) : AapCapability("battery", true, device),
                                                                    battery(true)
    {
        batteryChangedEventId = battery.GetBatteryChangedEvent().Subscribe([this](size_t id, std::vector<DeviceBatteryData> b){
            if (!isAvailable)
                isAvailable = true;

            _onChanged.FireEvent(*this);
            LOG_DEBUG("AapAncCapability::GetBatteryChangedEvent");
        });

        watcherBatteryChangedEventId = watcher.GetEvent().Subscribe([this](size_t id, std::vector<DeviceBatteryData> b){
            battery.UpdateBattery(b); });
    }

    AapBatteryCapability::~AapBatteryCapability()
    {
        battery.GetBatteryChangedEvent().Unsubscribe(batteryChangedEventId);
        watcher.GetEvent().Unsubscribe(watcherBatteryChangedEventId);
    }

}