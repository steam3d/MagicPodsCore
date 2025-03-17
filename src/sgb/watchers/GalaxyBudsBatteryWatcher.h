#pragma once

#include "../../Event.h"
#include "../enums/GalaxyBudsModelIds.h"
#include "../enums/GalaxyBudsMsgIds.h"
#include "../structs/GalaxyBudsResponseData.h"
#include "../../device/enums/DeviceBatteryType.h"
#include "../../device/enums/DeviceBatteryStatus.h"
#include "../../device/structs/DeviceBatteryData.h"
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
        bool IsCaseBatterySupport(GalaxyBudsModelIds model);
        void ProcessResponse(const GalaxyBudsResponseData &data);
        Event<std::vector<DeviceBatteryData>> &GetBatteryChangedEvent()
        {
            return _batteryChanged;
        }
    };
}