// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapBatteryWatcher.h"
#include "device/DeviceBattery.h"

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
        void OnReceivedData(const std::vector<unsigned char> &data) override;
        void Reset() override;

    public:
        explicit AapBatteryCapability(AapDevice& device);
        ~AapBatteryCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}