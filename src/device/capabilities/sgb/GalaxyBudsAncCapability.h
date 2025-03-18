#pragma once
#include "GalaxyBudsCapability.h"
#include "../../../sgb/watchers/GalaxyBudsAncWatcher.h"
#include "../../../sgb/setters/GalaxyBudsSetAnc.h"
#include "../../../sgb/GalaxyBudsPacket.h"
#include "../../enums/DeviceAncModes.h"

namespace MagicPodsCore
{
    class GalaxyBudsAncCapability : public GalaxyBudsCapability
    {
    private:
        DeviceAncModes option;
        GalaxyBudsAncWatcher watcher;
        size_t watcherAncChangedEventId;
        GalaxyBudsAnc DeviceAncModesToGalaxyBudsAnc(DeviceAncModes mode);
        DeviceAncModes GalaxyBudsAncToDeviceAncModes(GalaxyBudsAnc mode);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const GalaxyBudsResponseData &data);

    public:
        explicit GalaxyBudsAncCapability(GalaxyBudsDevice &device);
        ~GalaxyBudsAncCapability() override;
        void SetFromJson(nlohmann::json json) override;
    };
}