#pragma once
#include "../../Event.h"
#include "../enums/GalaxyBudsModelIds.h"
#include "../enums/GalaxyBudsMsgIds.h"
#include "../structs/GalaxyBudsResponseData.h"
#include "vector"

namespace MagicPodsCore
{
    // DO NOT USE IT LIKE THIS. ONLY FOR TESTING.
    // MUST BE REMOVED
    enum class DeviceBatteryType : unsigned char
    {
        Single = 0x01,
        Right = 0x02,
        Left = 0x04,
        Case = 0x08,
    };

    enum class DeviceBatteryStatus : unsigned char
    {
        // Hidden in UI
        NotAvailable = 0x00,
        // Opacity is zero in UI (usable for popup animation)
        Disconnected = 0x01,
        // Show in UI
        Connected = 0x02,
        // Showing battery last updated info even if battery already disconnected.
        Cached = 0x03,
    };

    struct DeviceBatteryData
    {
        DeviceBatteryType Type{};
        bool IsCharging{};
        DeviceBatteryStatus Status{};
        short Battery{};
    };
    //////////////////////////////////////////////////////////////////////////

    class GalaxyBudsBatteryWatcher
    {
    private:
        GalaxyBudsModelIds model;
        Event<std::vector<DeviceBatteryData>> _batteryChanged{};
        void ConvertBattery(std::vector<DeviceBatteryData> &battery, unsigned char l, unsigned char r, unsigned char c, unsigned char charging);
        std::vector<DeviceBatteryData> Process_EXTENDED_STATUS_UPDATED(GalaxyBudsResponseData data);
        std::vector<DeviceBatteryData> Process_STATUS_UPDATED(GalaxyBudsResponseData data);

    public:
        explicit GalaxyBudsBatteryWatcher(GalaxyBudsModelIds model) : model(model) {}
        bool IsCaseBatterySupport(GalaxyBudsModelIds model);
        void ProcessResponse(GalaxyBudsResponseData data);
        Event<std::vector<DeviceBatteryData>> &GetBatteryChangedEvent()
        {
            return _batteryChanged;
        }
    };
}