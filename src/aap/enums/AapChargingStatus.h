#pragma once

namespace MagicPodsCore
{
    enum class AapChargingStatus : unsigned char
    {
        // Got this when both AirPods out of the case
        Undefined = 0x00,
        Charging = 0x01,
        NotCharging = 0x02,
        Disconnected = 0x04,
    };

    static bool isValidAapChargingStatusType(unsigned char value){
        switch (static_cast<AapChargingStatus>(value)) {
            case AapChargingStatus::Undefined:
            case AapChargingStatus::Charging:
            case AapChargingStatus::NotCharging:
            case AapChargingStatus::Disconnected:
                return true;
            default:
                return false;
        }
    }

}