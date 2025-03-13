#include "GalaxyBudsBatteryWatcher.h"

namespace MagicPodsCore
{

    bool GalaxyBudsBatteryWatcher::IsCaseBatterySupport(GalaxyBudsModelIds model)
    {
        return !(model == GalaxyBudsModelIds::GalaxyBuds || model == GalaxyBudsModelIds::Unknown);
    }

    void GalaxyBudsBatteryWatcher::ConvertBattery(std::vector<DeviceBatteryData> &battery, unsigned char l, unsigned char r, unsigned char c, unsigned char charging)
    {

        DeviceBatteryData left;
        left.Type = DeviceBatteryType::Left;
        left.Battery = static_cast<short>(l);
        left.IsCharging = (charging & 0b00010000) != 0;
        left.Status = l == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected;
        battery.push_back(left);

        DeviceBatteryData right;
        right.Type = DeviceBatteryType::Right;
        right.Battery = static_cast<short>(r);
        right.IsCharging = (charging & 0b00000100) != 0;
        right.Status = r == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected;
        battery.push_back(right);

        if (c != 255)
        {
            DeviceBatteryData _case;
            _case.Type = DeviceBatteryType::Case;
            _case.Battery = static_cast<short>(c);
            _case.IsCharging = (charging & 0b00000001) != 0;
            _case.Status = c == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected;
            battery.push_back(_case);
        }
    }

    std::vector<DeviceBatteryData> GalaxyBudsBatteryWatcher::Process_EXTENDED_STATUS_UPDATED(GalaxyBudsResponseData data)
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

    std::vector<DeviceBatteryData> GalaxyBudsBatteryWatcher::Process_STATUS_UPDATED(GalaxyBudsResponseData data)
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

    void GalaxyBudsBatteryWatcher::ProcessResponse(GalaxyBudsResponseData data)
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