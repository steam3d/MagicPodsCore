#pragma once
#include "AapCapability.h"
#include "../../../aap/watchers/AapAncWatcher.h"
#include "../../../aap/setters/AapSetAnc.h"
#include "../../../aap/enums/AapAncMode.h"
#include "../../enums/DeviceAncModes.h"

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
        void OnReceivedData(const std::vector<unsigned char> &data);

    public:
        explicit AapAncCapability(AapDevice &device);
        ~AapAncCapability() override;
        void SetFromJson(nlohmann::json json) override;
    };
}