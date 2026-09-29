// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "PixelBudsCapability.h"
#include "sdk/pbp/watchers/PixelBudsBatteryWatcher.h"
#include "device/DeviceBattery.h"

namespace MagicPodsCore
{
    class PixelBudsBatteryCapability : public PixelBudsCapability
    {
    private:
        DeviceBattery battery;
        size_t batteryChangedEventId;

        PixelBudsBatteryWatcher watcher;
        size_t watcherBatteryChangedEventId;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const PwRpcPacket &packet);
        void Reset() override;

    public:
        explicit PixelBudsBatteryCapability(PixelBudsDevice& device);
        ~PixelBudsBatteryCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}
