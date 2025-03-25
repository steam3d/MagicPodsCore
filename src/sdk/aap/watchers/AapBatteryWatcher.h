#pragma once

#include "Event.h"
#include "device/structs/DeviceBatteryData.h"
#include "sdk/aap/enums/AapBatteryType.h"
#include "sdk/aap/enums/AapChargingStatus.h"
#include "AapWatcher.h"
#include <string>
#include <map>

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 04000400040003020164020104016301010801110201 AirPods 2
    // 0400040004000101015d0101                     AirPods Max
    //
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
    //                   ?  tr    r  rc    tl    l  lc    tc    c  cc
    // 04 00 04 00 04 00 03 02 01 64 02 01 04 01 63 01 01 08 01 11 02 01
    //                   ?  tl    l  lc    rl    r  rc    tc    c  cc
    // 04 00 04 00 04 00 03 04 01 63 02 01 02 01 00 04 01 08 01 00 04 01
    //                   ?  ?     s  sc
    // 04 00 04 00 04 00 01 01 01 5d 01 01
    //
    // 6 byte:
    // 01-03 - ? count of values
    //
    // 7 byte:
    // 02 - 9 bit shows right battery, 10 bit shows right charge status
    // 04 - 9 bit shows left battery, 10 bit shows left charge status
    // 01 - unknown possible AirPods Max only
    // 08 - case
    //
    // 9, 14 bytes:
    // 00    - possible shows that device is disconnected
    // 01-64 - charge level
    // ff    - unknown
    //
    // 10, 15 bytes:
    // 01 - charging
    // 02 - not charging
    // 04 - disconnected, the battery level ussualy is 00
    //
    // ---------------------------------------------------------------------------
    class AapBatteryWatcher : public AapWatcher
    {
    private:
        Event<std::vector<DeviceBatteryData>> _event{};
        void ConvertBattery(std::vector<DeviceBatteryData> &battery, AapBatteryType type, AapChargingStatus status, unsigned char charge);

    public:
        AapBatteryWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<std::vector<DeviceBatteryData>> &GetEvent()
        {
            return _event;
        }
    };

}