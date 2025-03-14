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
    void AapBatteryWatcher::ProcessBytes(const std::vector<unsigned char> &bytes)
    {
        // min length of the battery packet
        if (bytes.size() < 11)
            return;

        // packet type must be battery
        if (bytes[4] != static_cast<unsigned char>(AapCmd::Battery))
            return;

        LOG_DEBUG("%s", _tag.c_str());

        int batteryCount = static_cast<int>(bytes[6]);

        int startByte = 7;

        std::string readableStr = "";
        std::vector<DeviceBatteryData> batteryData;

        for (int i = 0; i < batteryCount; i++)
        {
            if (!isValidAapBatteryType(bytes[startByte]))
                continue;

            AapBatteryType batteryType = static_cast<AapBatteryType>(bytes[startByte]);
            unsigned char battery = bytes[startByte + 2];

            if (!isValidAapChargingStatusType(bytes[startByte]))
                continue;

            AapChargingStatus charging = static_cast<AapChargingStatus>(bytes[startByte + 3]);
            startByte += 5;
            
            ConvertBattery(batteryData, batteryType, charging, battery);
            readableStr += DummyConvertBatteryType(batteryType) + " " + std::to_string(battery) + " " + DummyConvertChargingStatus(charging) + "";
        }

        if (batteryData.size() > 0)
        {
            _event.FireEvent(batteryData);
            LOG_DEBUG("%s", readableStr.c_str());
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