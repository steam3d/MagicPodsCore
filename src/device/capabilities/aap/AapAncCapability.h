#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapAncWatcher.h"
#include "sdk/aap/enums/AapAncMode.h"
#include "device/enums/DeviceAncModes.h"

namespace MagicPodsCore
{
    class AapAncCapability : public AapCapability
    {
    private:
        DeviceAncModes option;
        AapAncWatcher watcher{};
        size_t watcherAncChangedEventId;        
        AapAncMode DeviceAncModesToAapAncMode(DeviceAncModes mode);
        DeviceAncModes AapAncModeToDeviceAncModes(AapAncMode mode);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapAncCapability(AapDevice& device);
        ~AapAncCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}