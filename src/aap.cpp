#include<stdio.h>
#include <iostream>
#include <string>
using namespace std;

#include "appleproductids.h"
#include "vendorids.h"

// Byte 4 indicates the packet type
enum class Cmd: unsigned char
{    
    Battery = 0x04,
    // Request to change or response that setting in headphones was changed
    Settings = 0x09,
    Notifications = 0x0f,
};

// Byte 6 indicates the setting type
enum class CmdSettings: unsigned char
{    
    Anc = 0x0d,
};

// Base class to get request that will be send to headphones
class AapRequest
{    
    public:        
        virtual unsigned char* Request(){
            return new unsigned char[length];
        }

        int GetLength(){
            return length;
        }

        void PrintAsHex(){
            printf("%s \n", tag.c_str());
            unsigned char* bArr = Request();
            for(int i = 0; i < length; i++)
                printf("%02x", bArr[i]);
            printf("\n");
            printf("\n");
        }
    protected:
        int length = 0;
        string tag = "AapRequest";

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
class AapInit: public AapRequest
{
    public:
        AapInit(){
            tag = "AapInit";
        }
        unsigned char* Request() override{
            length = 16; //
            return new unsigned char[length] {0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        }
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
enum class NotificationsMode: unsigned char
{
    // Always use this
    Unknown1 = 0xff, 
    Unknown2 = 0xef,
};

class AapEnableNotifications: public AapRequest
{
    public:
        AapEnableNotifications(NotificationsMode mode){
            this->mode = mode;
            this->tag = "AapEnableNotifications";
        }
        unsigned char* Request() override{
            length = 10; //
            return new unsigned char[length] {0x00, 0x00, 0x04, 0x00, (unsigned char)Cmd::Notifications, 0x00, 0xff, 0xff, (unsigned char)mode, 0xff};
        }
    private:
        NotificationsMode mode;
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
enum class AncMode
{    
    Off = 0x01,
    On = 0x02, 
    Tra = 0x03
};

class AapSetAnc: public AapRequest
{
    public:
        AapSetAnc(AncMode mode){
            this->mode = mode;
            this->tag = "AapSetAnc";
        }
        unsigned char* Request() override{
            length = 11;
            return new unsigned char[length] {0x04, 0x00, 0x04, 0x00, (unsigned char)Cmd::Settings, 0x00, (unsigned char)CmdSettings::Anc, (unsigned char)mode, 0x00, 0x00, 0x00};
        }
    private:
        AncMode mode;
};

// Base class to process incoming data from headphones
class AapWatcher
{
    public:
    //Subscribe each class to process incoming data from headphones
    virtual void ProcessBytes(unsigned char* bArr, int length){
        return;
    };
    
    protected:        
        string tag = "AapWatcher";
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
enum class BatteryType: unsigned char
{
    Single = 0x01, 
    Right = 0x02, 
    Left = 0x04, 
    Case = 0x08, 
};

enum class ChargingStatus: unsigned char
{
    Charging = 0x01, 
    NotCharging = 0x02, 
    Disconnected = 0x04, 
};

class AapBatteryWatcher: public AapWatcher
{
    public:
        AapBatteryWatcher(){
            tag = "AapBatteryWatcher";
        }

        
        // !MUST BE TESTED ON ALL AIRPOD MODES! 
        void ProcessBytes(unsigned char* bArr, int length) override{
            // min length of the battery packet
            if (length < 11) return;

            // packet type must be battery
            if (bArr[4] != (unsigned char)Cmd::Battery) return;
            
            std::cout << tag << std::endl;
            
            int batteryCount = (int)bArr[6];

            int startByte = 7;
            std::string readableStr = "";
            for(int i = 0; i < batteryCount; i++){
                BatteryType batteryType = static_cast<BatteryType>(bArr[startByte]);
                unsigned char battery = bArr[startByte + 2];
                ChargingStatus charging = static_cast<ChargingStatus>(bArr[startByte+3]);
                startByte += 5;
                
                // REPLACE WITH BATTERY STORAGE LOGIC OR EVENT ONBATTERYCHANGED?
                readableStr+= DummyConvertBatteryType(batteryType) + " " + std::to_string(battery) + " " + DummyConvertChargingStatus(charging) + "\n";                                
            }

            std::cout << readableStr << std::endl;
        }

    private:
        std::string DummyConvertChargingStatus(ChargingStatus status){
            switch (status) {
                    case ChargingStatus::Charging:
                        return "Charging";
                    case ChargingStatus::NotCharging:
                        return "NotCharging";
                    case ChargingStatus::Disconnected:
                        return "Disconnected";
                    default:
                        return "Unknown";
            }
        }

        std::string DummyConvertBatteryType(BatteryType status){
            switch (status) {
                    case BatteryType::Single:
                        return "Single";
                    case BatteryType::Left:
                        return "Left  ";
                    case BatteryType::Right:
                        return "Right ";
                    case BatteryType::Case:
                        return "Case  "; 
                    default:
                        return "Unknown";
            }
        }

};

class AapAncWatcher: public AapWatcher
{
    public:
        AapAncWatcher(){
            this->tag = "AapAncWatcher";
        }
         void ProcessBytes(unsigned char* bArr, int length) override{
            // min length of the battery packet
            if (length < 10) return;

            // packet type must be battery
            if (bArr[4] != (unsigned char)Cmd::Settings) return;
            
            std::cout << tag << std::endl;

            AncMode ancMode = static_cast<AncMode>(bArr[7]);

             // REPLACE WITH ANC STORAGE LOGIC OR EVENT ONANCCHANGED?
            std::cout << DummyConvertAncMode(ancMode) << std::endl;
        }
    private:
        std::string DummyConvertAncMode(AncMode mode){
        switch (mode) {
                case AncMode::On:
                    return "On";
                case AncMode::Off:
                    return "Off";
                case AncMode::Tra:
                    return "Tra"; 
                default:
                    return "Unknown";
            }
        }   
};

/* demo.c:  My first C program on a Linux */
int main(void)
{
    AapInit init;
    init.PrintAsHex();

    AapEnableNotifications EnableNotifications(NotificationsMode::Unknown1);
    EnableNotifications.PrintAsHex();

    AapSetAnc setAnc(AncMode::Off);
    setAnc.PrintAsHex();

    //04000400040003020164020104016301010801110201
    unsigned char* barr22 = new unsigned char[22] { 4, 0, 4, 0, 4, 0, 3, 2, 1, 100, 2, 1, 4, 1, 99, 1, 1, 8, 1, 17, 2, 1};
    //0400040004000101015d0101
    unsigned char* barr12 = new unsigned char[22] { 4, 0, 4, 0, 4, 0, 1, 1, 1, 93, 1, 1};
    AapBatteryWatcher batteryWatcher;
    batteryWatcher.ProcessBytes(barr22, 22);
    batteryWatcher.ProcessBytes(barr12, 22);

    AapAncWatcher aapAncWatcher;
    aapAncWatcher.ProcessBytes(setAnc.Request(), setAnc.GetLength());

     printf("%04x \n", 0x4c00);
     printf("%04x \n", 0x4c05);
     printf("%d \n", 0x4c00);
     printf("%d \n", AppleProductIds::airpods2);
     printf("%04x \n", VendorIds::Apple);

    return 0;
}


