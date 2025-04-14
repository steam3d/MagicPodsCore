// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

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

    static std::string AapChargingStatusToString(AapChargingStatus status)
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
}