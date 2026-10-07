// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "GalaxyBudsCapability.h"
#include "sdk/sgb/watchers/GalaxyBudsEarDetectionWatcher.h"
#include "device/structs/DeviceEarDetectionData.h"

namespace MagicPodsCore
{
    class GalaxyBudsEarDetectionCapability : public GalaxyBudsCapability
    {
    private:
        DeviceEarDetectionData option;
        GalaxyBudsEarDetectionWatcher watcher;
        size_t watcherEarChangedEventId;        
        DeviceEarDetectionData GalaxyBudsEarDetectionToDeviceEarDetectionData(GalaxyBudsEarDetectionStateArgs mode);
        void TriggerMediaControl();
        bool LoadSetting();
        void SaveSetting(bool b);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const GalaxyBudsResponseData &data);

    public:
        explicit GalaxyBudsEarDetectionCapability(GalaxyBudsDevice& device);
        ~GalaxyBudsEarDetectionCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}