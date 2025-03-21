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

    void AapBatteryCapability::Reset()
    {
        battery.ClearBattery();
        AapCapability::Reset();
    }

    AapBatteryCapability::AapBatteryCapability(AapDevice &device) : AapCapability("battery", true, device),
                                                                    battery(true)
    {
        batteryChangedEventId = battery.GetBatteryChangedEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            if (!isAvailable)
                isAvailable = true;

            _onChanged.FireEvent(*this);
        });

        watcherBatteryChangedEventId = watcher.GetEvent().Subscribe([this](size_t id, const std::vector<DeviceBatteryData> &b){
            battery.UpdateBattery(b);

        });
    }

    AapBatteryCapability::~AapBatteryCapability()
    {
        battery.GetBatteryChangedEvent().Unsubscribe(batteryChangedEventId);
        watcher.GetEvent().Unsubscribe(watcherBatteryChangedEventId);
    }

}