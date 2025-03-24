#pragma once
#include "GalaxyBudsCapability.h"
#include "sdk/sgb/watchers/GalaxyBudsBatteryWatcher.h"
#include "device/DeviceBattery.h"

namespace MagicPodsCore
{
    class GalaxyBudsBatteryCapability : public GalaxyBudsCapability
    {
    private:
        DeviceBattery battery;
        size_t batteryChangedEventId;

        GalaxyBudsBatteryWatcher watcher;
        size_t watcherBatteryChangedEventId;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const GalaxyBudsResponseData &data);

    public:
        explicit GalaxyBudsBatteryCapability(GalaxyBudsDevice &device);
        ~GalaxyBudsBatteryCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}