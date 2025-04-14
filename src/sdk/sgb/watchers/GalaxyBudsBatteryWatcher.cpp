// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsBatteryWatcher.h"
#include "sdk/sgb/enums/GalaxyBudsMsgIds.h"
#include "device/enums/DeviceBatteryType.h"
#include "device/enums/DeviceBatteryStatus.h"

namespace MagicPodsCore
{

    bool GalaxyBudsBatteryWatcher::IsCaseBatterySupport(GalaxyBudsModelIds model)
    {
        return !(model == GalaxyBudsModelIds::GalaxyBuds || model == GalaxyBudsModelIds::Unknown);
    }

    void GalaxyBudsBatteryWatcher::ConvertBattery(std::vector<DeviceBatteryData> &battery, unsigned char l, unsigned char r, unsigned char c, unsigned char charging)
    {

        DeviceBatteryData left(
            DeviceBatteryType::Left,
            l == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
            static_cast<short>(l),
            (charging & 0b00010000) != 0);
        battery.push_back(left);

        DeviceBatteryData right(
            DeviceBatteryType::Right,
            r == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
            static_cast<short>(r),
            (charging & 0b00000100) != 0);
        battery.push_back(right);

        if (c != 255)
        {
            DeviceBatteryData _case(
                DeviceBatteryType::Case,
                c == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                static_cast<short>(c),
                (charging & 0b00000001) != 0);
            battery.push_back(_case);
        }
    }

    std::vector<DeviceBatteryData> GalaxyBudsBatteryWatcher::Process_EXTENDED_STATUS_UPDATED(const GalaxyBudsResponseData &data)
    {

        std::vector<DeviceBatteryData> battery;
        if (data.Id != GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED)
        {
            return battery;
        };

        if (data.Payload.size() <= 3)
        {
            return battery;
        }

        unsigned char revision = data.Payload[0];
        unsigned char batteryL = data.Payload[2];
        unsigned char batteryR = data.Payload[3];

        unsigned char batteryC = 255; // Custom state. Case is not available. 225 = -1
        unsigned char chargingStatus = 0;

        if (model == GalaxyBudsModelIds::GalaxyBudsPlus)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsLive)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPro)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds2)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
            if (data.Payload.size() > 36 && revision >= 10)
                chargingStatus = data.Payload[36];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds2Pro)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
            if (data.Payload.size() > 43 && revision >= 11)
                chargingStatus = data.Payload[43];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsFe)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
            if (data.Payload.size() > 43)
                chargingStatus = data.Payload[43];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds3)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
            if (data.Payload.size() > 42)
                chargingStatus = data.Payload[42];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds3Pro)
        {
            if (data.Payload.size() > 7)
                batteryC = data.Payload[7];
            if (data.Payload.size() > 42)
                chargingStatus = data.Payload[42];
        }

        ConvertBattery(battery, batteryL, batteryR, batteryC, chargingStatus);
        return battery;
    }

    std::vector<DeviceBatteryData> GalaxyBudsBatteryWatcher::Process_STATUS_UPDATED(const GalaxyBudsResponseData &data)
    {
        std::vector<DeviceBatteryData> battery;

        if (data.Id != GalaxyBudsMsgIds::STATUS_UPDATED)
        {
            return battery;
        }

        if (data.Payload.size() <= 3)
        {
            return battery;
        }

        unsigned char batteryL = data.Payload[1];
        unsigned char batteryR = data.Payload[2];

        unsigned char batteryC = 255;     // Custom state. Case is not available. 225 = -1
        unsigned char chargingStatus = 0; // Custom state. Default charging state is not charging

        if (model == GalaxyBudsModelIds::GalaxyBudsPlus)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsLive)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPro)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds2)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
            if (data.Payload.size() > 7)
                chargingStatus = data.Payload[7];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds2Pro)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
            if (data.Payload.size() > 7)
                chargingStatus = data.Payload[7];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsFe)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
            if (data.Payload.size() > 7)
                chargingStatus = data.Payload[7];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds3)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
            if (data.Payload.size() > 7)
                chargingStatus = data.Payload[7];
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds3Pro)
        {
            if (data.Payload.size() > 6)
                batteryC = data.Payload[6];
            if (data.Payload.size() > 7)
                chargingStatus = data.Payload[7];
        }

        ConvertBattery(battery, batteryL, batteryR, batteryC, chargingStatus);
        return battery;
    }

    void GalaxyBudsBatteryWatcher::ProcessResponse(const GalaxyBudsResponseData &data)
    {

        if (data.Id == GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED)
        {
            std::vector<DeviceBatteryData> battery = Process_EXTENDED_STATUS_UPDATED(data);
            if (battery.size() > 0)
            {
                _batteryChanged.FireEvent(battery);
            }
        }
        else if (data.Id == GalaxyBudsMsgIds::STATUS_UPDATED)
        {
            std::vector<DeviceBatteryData> battery = Process_STATUS_UPDATED(data);
            if (battery.size() > 0)
            {
                _batteryChanged.FireEvent(battery);
            }
        }
    }
}