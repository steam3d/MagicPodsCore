// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "GalaxyBudsCapability.h"
#include "sdk/sgb/watchers/GalaxyBudsAncWatcher.h"
#include "device/enums/DeviceAncModes.h"

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
        explicit GalaxyBudsAncCapability(GalaxyBudsDevice& device);
        ~GalaxyBudsAncCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}