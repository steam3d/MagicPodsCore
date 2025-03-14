#pragma once

namespace MagicPodsCore
{
    enum class AapBatteryType : unsigned char
    {
        Single = 0x01,
        Right = 0x02,
        Left = 0x04,
        Case = 0x08,
    };

    static bool isValidAapBatteryType(unsigned char value){
        switch (static_cast<AapBatteryType>(value)) {
            case AapBatteryType::Single:
            case AapBatteryType::Right:
            case AapBatteryType::Left:
            case AapBatteryType::Case:
                return true;
            default:
                return false;
        }
    }

}