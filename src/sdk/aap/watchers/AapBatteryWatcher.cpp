#include "AapBatteryWatcher.h"
#include "sdk/aap/enums/AapCmd.h"

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
            break;
        case AapBatteryType::Left:
            dtype = DeviceBatteryType::Left;
            break;
        case AapBatteryType::Right:
            dtype = DeviceBatteryType::Right;
            break;
        case AapBatteryType::Case:
            dtype = DeviceBatteryType::Case;
            break;
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
            
            //Fake AirPods send a value of 127 after the case is closed. Should we ignore values greater than 100?
            unsigned char battery = data[startByte + 2];

            if (!isValidAapChargingStatusType(data[startByte + 3]))
                continue;

            AapChargingStatus charging = static_cast<AapChargingStatus>(data[startByte + 3]);
            startByte += 5;
            
            ConvertBattery(batteryData, batteryType, charging, battery);
            readableStr += AapBatteryTypeToString(batteryType) + " " + std::to_string(battery) + " " + AapChargingStatusToString(charging) + " ";
        }

        if (batteryData.size() > 0)
        {
            Logger::Debug("%s: %s", _tag.c_str(), readableStr.c_str());
            _event.FireEvent(batteryData);
        }
    }

}