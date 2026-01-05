// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "../Capability.h"
#include "device/DeviceBattery.h"
#include "device/BhfDevice.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace MagicPodsCore
{
    class BhfBatteryCapability : public Capability
    {
    private:
        DeviceBattery battery;
        size_t onConnectedPropertyChangedId;
        size_t onHandsFreeBatteryPropertyChangedId;
        size_t batteryChangedEventId;
        void UpdateSingleBattery(uint8_t level);
    protected:
        BhfDevice& device;
        nlohmann::json CreateJsonBody() override;
        void Reset() override;

    public:
        explicit BhfBatteryCapability(BhfDevice& device);
        ~BhfBatteryCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}
