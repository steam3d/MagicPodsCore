
#include "SonyGetHeadphonesBattery.h"
#include "Logger.h"

namespace MagicPodsCore
{
    SonyGetHeadphonesBattery::SonyGetHeadphonesBattery() : SonyBaseCmd{"SonyGetHeadphonesBattery"}
    {
    }

    std::vector<unsigned char> SonyGetHeadphonesBattery::CreatePacketBody(unsigned char prefix) const
    {
        return std::vector<unsigned char>{0x0c, prefix, 0x00, 0x00, 0x00, 0x02, 0x10, 0x01};
    }

    void SonyGetHeadphonesBattery::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        if (bytes.size() == 15 && bytes[6] == 6 && bytes[7] == 17)
        {
            auto l = bytes[9];
            auto r = bytes[11];

            DeviceBatteryData batteryL;
            batteryL.isCharging = false;
            batteryL.Status = l == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected;
            batteryL.Battery = l;

            DeviceBatteryData batteryR;
            batteryR.isCharging = false;
            batteryR.Status = r == 0 ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected;
            batteryR.Battery = r;

            Battery[DeviceBatteryType::Left] = batteryL;
            Battery[DeviceBatteryType::Right] = batteryR;

            // Event battery changed
            LOG_DEBUG("Battery Left: %d Right %d", batteryL.Battery, batteryR.Battery);
            SonyBaseCmd::ProcessResponse(bytes);
        }
    }
}