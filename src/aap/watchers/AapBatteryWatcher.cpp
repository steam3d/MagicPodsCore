#include "AapBatteryWatcher.h"

namespace MagicPodsCore
{
    AapBatteryWatcher::AapBatteryWatcher() : AapWatcher{"AapBatteryWatcher"}
    {
    }

    void AapBatteryWatcher::ConvertBattery(std::vector<DeviceBatteryData> &batteryData, AapBatteryType type, AapChargingStatus status, unsigned char battery)
    {        
        DeviceBatteryType dtype;
        switch (type)
        {
        case AapBatteryType::Single:
            dtype = DeviceBatteryType::Single;
        case AapBatteryType::Left:
            dtype = DeviceBatteryType::Left;
        case AapBatteryType::Right:
            dtype = DeviceBatteryType::Right;
        case AapBatteryType::Case:
            dtype = DeviceBatteryType::Case;
        }

        DeviceBatteryData data(
            dtype,
            status == AapChargingStatus::Disconnected ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
            battery,
            status == AapChargingStatus::Charging
        );

        batteryData.push_back(data);
    }

    // !MUST BE TESTED ON ALL AIRPOD MODES!
    void AapBatteryWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        // min length of the battery packet
        if (data.size() < 11)
            return;

        // packet type must be battery
        if (data[4] != static_cast<unsigned char>(AapCmd::Battery))
            return;        

        int batteryCount = static_cast<int>(data[6]);

        int startByte = 7;

        std::string readableStr = "";
        std::vector<DeviceBatteryData> batteryData;

        for (int i = 0; i < batteryCount; i++)
        {
            if (!isValidAapBatteryType(data[startByte]))
                continue;

            AapBatteryType batteryType = static_cast<AapBatteryType>(data[startByte]);
            unsigned char battery = data[startByte + 2];

            if (!isValidAapChargingStatusType(data[startByte]))
                continue;

            AapChargingStatus charging = static_cast<AapChargingStatus>(data[startByte + 3]);
            startByte += 5;
            
            ConvertBattery(batteryData, batteryType, charging, battery);
            readableStr += DummyConvertBatteryType(batteryType) + " " + std::to_string(battery) + " " + DummyConvertChargingStatus(charging) + " ";
        }

        if (batteryData.size() > 0)
        {
            LOG_DEBUG("%s: %s", _tag.c_str(), readableStr.c_str());
            _event.FireEvent(batteryData);
        }
    }

    std::string AapBatteryWatcher::DummyConvertChargingStatus(AapChargingStatus status)
    {
        switch (status)
        {
        case AapChargingStatus::Undefined:
            return "Undefined";
        case AapChargingStatus::Charging:
            return "Charging";
        case AapChargingStatus::NotCharging:
            return "NotCharging";
        case AapChargingStatus::Disconnected:
            return "Disconnected";
        default:
            return "Unknown";
        }
    }

    std::string AapBatteryWatcher::DummyConvertBatteryType(AapBatteryType status)
    {
        switch (status)
        {
        case AapBatteryType::Single:
            return "Single";
        case AapBatteryType::Left:
            return "Left  ";
        case AapBatteryType::Right:
            return "Right ";
        case AapBatteryType::Case:
            return "Case  ";
        default:
            return "Unknown";
        }
    }

}