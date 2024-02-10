#pragma once

#include "AppleProductIds.h"
#include "BtVendorIds.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

namespace MagicPodsCore {

    // Byte 4 indicates the packet type
    enum class Cmd : unsigned char {    
        Battery = 0x04,
        // Request to change or response that setting in headphones was changed
        Settings = 0x09,
        Notifications = 0x0f,
    };

    // Byte 6 indicates the setting type
    enum class CmdSettings : unsigned char {    
        Anc = 0x0d,
    };

    // Base class to get request that will be send to headphones
    class AapRequest {    
    protected:
        std::string _tag{};

    public:
        explicit AapRequest(std::string tag);
        virtual std::vector<unsigned char> Request() const = 0;
        void PrintAsHex();
    };

    // ---------------------------------------------------------------------------
    // Initialization handshake with aap service
    // 00000400010002000000000000000000
    //
    // Also working (not recommend to use)
    // 00000400000000000000000000000000
    //
    // Responce (Unknown ussage) 
    // 010004000000010002000500494e0500a54f
    // ---------------------------------------------------------------------------
    class AapInit : public AapRequest {
    public:
        AapInit();
        std::vector<unsigned char> Request() const override;
    };


    // ---------------------------------------------------------------------------
    // Request to headphones provide notification:
    // Set Notification Filter
    // Battery: Enabled
    // In Ear: Enabled
    // Role: Enabled
    // Connection: Enabled
    // Stream: Enabled
    // Relay: Enabled
    // Easy Pair: Enabled
    // Triangle: Enabled
    // Custom Message: Enabled
    //
    // iPhone sends first this:
    // 040004000f00fffffeff
    //
    // Then iPhone request capabilities and sends this: 
    // 040004000f00ffffffff
    // 0  1  2  3  4  5  6  7  8  9 
    //                         un
    // 04 00 04 00 0f 00 ff ff ff ff
    // 04 00 04 00 0f 00 ff ff fe ff
    //
    // ! It is unknown which bytes are responsible for what !
    // ---------------------------------------------------------------------------

    // Byte 8
    enum class NotificationsMode : unsigned char {
        // Always use this
        Unknown1 = 0xff, 
        Unknown2 = 0xef,
    };

    class AapEnableNotifications : public AapRequest {
    private:
        NotificationsMode _mode{};

    public:
        AapEnableNotifications(NotificationsMode mode);
        std::vector<unsigned char> Request() const override;
    };


    // ---------------------------------------------------------------------------
    // 0400040009000d01000000
    // 
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 0d 01 00 00 00
    // 04 00 04 00 09 00 0d 02 00 00 00
    // 04 00 04 00 09 00 0d 03 00 00 00
    //
    // 7 byte:
    // 01 - off
    // 02 - on
    // 03 - transparency
    //
    // Request and responce the same. 
    // Headphones send the same packet after change anc mode.    
    // ---------------------------------------------------------------------------

    // Byte 7 indicates the anc mode
    enum class AncMode {    
        Off = 0x01,
        On = 0x02, 
        Tra = 0x03
    };

    class AapSetAnc: public AapRequest {
    private:
        AncMode _mode;

    public:
        AapSetAnc(AncMode mode);
        std::vector<unsigned char> Request() const override;
    };

    // Base class to process incoming data from headphones
    class AapWatcher {
    protected:        
        std::string _tag {};

    public:
        AapWatcher(std::string tag);
        // Subscribe each class to process incoming data from headphones
        virtual void ProcessBytes(const std::vector<unsigned char>& bytes) = 0;
    };


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
    enum class BatteryType: unsigned char {
        Single = 0x01, 
        Right = 0x02, 
        Left = 0x04, 
        Case = 0x08, 
    };

    enum class ChargingStatus: unsigned char {
        Charging = 0x01, 
        NotCharging = 0x02, 
        Disconnected = 0x04, 
    };

    class AapBatteryWatcher : public AapWatcher {
    public:
        AapBatteryWatcher();
        // !MUST BE TESTED ON ALL AIRPOD MODES! 
        void ProcessBytes(const std::vector<unsigned char>& bytes) override;

    private:
        std::string DummyConvertChargingStatus(ChargingStatus status);
        std::string DummyConvertBatteryType(BatteryType status);
    };

    class AapAncWatcher : public AapWatcher {
    public:
        AapAncWatcher();
        void ProcessBytes(const std::vector<unsigned char>& bytes) override;

    private:
        std::string DummyConvertAncMode(AncMode mode);
    };

}