#pragma once
#include "AapCapability.h"
#include "../../../aap/watchers/AapBatteryWatcher.h"
#include "../../DeviceBattery.h"

namespace MagicPodsCore
{
    class AapBatteryCapability : public AapCapability
    {
    private:
        DeviceBattery battery;
        size_t batteryChangedEventId;
        
        AapBatteryWatcher watcher{};
        size_t watcherBatteryChangedEventId;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data);

    public:
        explicit AapBatteryCapability(AapDevice &device);
        ~AapBatteryCapability() override;
        void SetFromJson(nlohmann::json json) override {};
    };
}