// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
#include "device/structs/DeviceBatteryData.h"
#include "vector"

namespace MagicPodsCore
{
    class GalaxyBudsBatteryWatcher
    {
    private:
        GalaxyBudsModelIds model;
        Event<std::vector<DeviceBatteryData>> _batteryChanged{};
        void ConvertBattery(std::vector<DeviceBatteryData> &battery, unsigned char l, unsigned char r, unsigned char c, unsigned char charging);
        std::vector<DeviceBatteryData> Process_EXTENDED_STATUS_UPDATED(const GalaxyBudsResponseData &data);
        std::vector<DeviceBatteryData> Process_STATUS_UPDATED(const GalaxyBudsResponseData &data);

    public:
        explicit GalaxyBudsBatteryWatcher(GalaxyBudsModelIds model) : model(model) {}
        void ProcessResponse(const GalaxyBudsResponseData &data);
        Event<std::vector<DeviceBatteryData>> &GetBatteryChangedEvent()
        {
            return _batteryChanged;
        }
        static bool IsCaseBatterySupport(GalaxyBudsModelIds model);
    };
}