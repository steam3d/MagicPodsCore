
#include "SonyGetCaseBattery.h"
#include "Logger.h"


namespace MagicPodsCore
{
    SonyGetCaseBattery::SonyGetCaseBattery() : SonyBaseCmd{"SonyGetCaseBattery"}
    {
    }

    std::vector<unsigned char> SonyGetCaseBattery::CreatePacketBody(unsigned char prefix) const
    {
        return std::vector<unsigned char>{0x0c, prefix, 0x00, 0x00, 0x00, 0x02, 0x10, 0x02};
    }

    void SonyGetCaseBattery::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        if (bytes.size() == 13 && bytes[6] == 4 && bytes[7] == 17)
        {            
            auto c = bytes[9];

            DeviceBatteryData battery;
            battery.isCharging = false;
            battery.Status = DeviceBatteryStatus::Cached;
            battery.Battery = bytes[9];

            Battery[DeviceBatteryType::Case] = battery;
            // Event battery changed
            LOG_DEBUG("Battery case: %d", battery.Battery);
            SonyBaseCmd::ProcessResponse(bytes);
        }
    }
}