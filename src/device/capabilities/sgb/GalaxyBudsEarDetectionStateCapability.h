// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "GalaxyBudsCapability.h"
#include "device/structs/DeviceEarDetectionData.h"
#include "sdk/sgb/watchers/GalaxyBudsEarDetectionWatcher.h"

namespace MagicPodsCore
{
    class GalaxyBudsEarDetectionStateCapability : public GalaxyBudsCapability
    {
    private:
        DeviceEarDetectionData option;
        GalaxyBudsEarDetectionWatcher watcher;
        size_t watcherEventId;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const GalaxyBudsResponseData &data) override;

    public:
        explicit GalaxyBudsEarDetectionStateCapability(GalaxyBudsDevice& device);
        ~GalaxyBudsEarDetectionStateCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}
