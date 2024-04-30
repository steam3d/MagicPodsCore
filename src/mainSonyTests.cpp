#include <iostream>
#include <iomanip>
#include <vector>
#include "Logger.h"
#include <sstream>
#include "sony/SonyBaseCmd.h"
#include "sony/SonyGetAnc.h"
#include "sony/SonyGetCaseBattery.h"
#include "sony/SonyGetHeadphonesBattery.h"
#include "sony/SonySetAnc.h"
#include "sony/SonyAncWatcher.h"
using namespace MagicPodsCore;

void PrintBytes(std::vector<unsigned char> bytes)
{
    #ifdef DEBUG
    printf("    ");
    for (int i = 0; i < bytes.size(); i++)
    {
        printf("%02x", bytes[i]);
    }
    printf("\n");
    #endif
}

void TestRequest(SonyBaseCmd &cmd, std::vector<unsigned char> bytesP0, std::vector<unsigned char> bytesP1)
{
    PrintBytes(cmd.Request(0));
    if (cmd.Request(0) == bytesP0)
    {
        LOG_RELEASE("    [ ] Prefix 0");
    }
    else
    {
        LOG_RELEASE("    [x] Prefix 0");
    }

    PrintBytes(cmd.Request(1));
    if (cmd.Request(1) == bytesP1)
    {
        LOG_RELEASE("    [ ] Prefix 1");
    }
    else
    {
        LOG_RELEASE("    [x] Prefix 1");
    }
}

void CompareAncStates(bool isProcessed, SonyAncState stateActual, SonyAncState stateExpected){
    if (isProcessed)
    {
        LOG_RELEASE("    [ ] isProcessed");
        if (stateActual.AmbientVoice == stateExpected.AmbientVoice)
        {
            LOG_RELEASE("    [ ] AmbientVoice");
        }
        else
        {
            LOG_RELEASE("    [x] AmbientVoice");
        }

        if (stateActual.AncFilter == stateExpected.AncFilter)
        {
            LOG_RELEASE("    [ ] AncFilter");
        }
        else
        {
            LOG_RELEASE("    [x] AncFilter");
        }

        if (stateActual.AncSwitch == stateExpected.AncSwitch)
        {
            LOG_RELEASE("    [ ] AncSwitch");
        }
        else
        {
            LOG_RELEASE("    [x] AncSwitch");
        }

        if (stateActual.Volume == stateExpected.Volume)
        {
            LOG_RELEASE("    [ ] Volume");
        }
        else
        {
            LOG_RELEASE("    [x] Volume");
        }
    }
    else
    {
        LOG_RELEASE("    [x] isProcessed");
    }
}

void TestSonyGetAncResponse(std::vector<unsigned char> bytes, SonyAncState state)
{
    SonyGetAnc cmd = SonyGetAnc();
    cmd.ProcessResponse(bytes);
    CompareAncStates(cmd.IsProcessed, cmd.state, state);

}

void TestSonyAncWatcher(std::vector<unsigned char> bytes, SonyAncState state)
{
    SonyAncWatcher cmd = SonyAncWatcher();
    cmd.ProcessResponse(bytes);
    CompareAncStates(true, cmd.State, state);

}


std::string DummyConvertDeviceBatteryType(DeviceBatteryType status)
{
    switch (status)
    {
    case DeviceBatteryType::Case:
        return "Case";
    case DeviceBatteryType::Left:
        return "Left";
    case DeviceBatteryType::Right:
        return "Right";
    case DeviceBatteryType::Single:
        return "Single";
    default:
        return "Unknown";
    }
}

void TestSonyGetCaseBatteryResponse(std::vector<unsigned char> bytes, std::map<DeviceBatteryType, DeviceBatteryData> Battery)
{
    SonyGetCaseBattery cmd = SonyGetCaseBattery();
    cmd.ProcessResponse(bytes);

    if (cmd.IsProcessed)
    {
        LOG_RELEASE("    [ ] isProcessed");
        for (auto const &b : Battery)
        {
            if (cmd.Battery.contains(b.first) && cmd.Battery[b.first].Battery == b.second.Battery)
            {
                LOG_RELEASE("    [ ] Battery %s: %d", DummyConvertDeviceBatteryType(b.first).c_str(), cmd.Battery[b.first].Battery);
            }
            else
            {
                LOG_RELEASE("    [x] Battery %s", DummyConvertDeviceBatteryType(b.first).c_str());
            }
        }
    }
    else
    {
        LOG_RELEASE("    [x] isProcessed");
    }
}

void TestSonyGetHeadphonesBatteryResponse(SonyProductIds model, std::vector<unsigned char> bytes, std::map<DeviceBatteryType, DeviceBatteryData> Battery)
{
    SonyGetHeadphonesBattery cmd = SonyGetHeadphonesBattery(model);
    cmd.ProcessResponse(bytes);

    if (cmd.IsProcessed)
    {
        LOG_RELEASE("    [ ] isProcessed");
        for (auto const &b : Battery)
        {
            if (cmd.Battery.contains(b.first) && cmd.Battery[b.first].Battery == b.second.Battery)
            {
                LOG_RELEASE("    [ ] Battery %s: %d", DummyConvertDeviceBatteryType(b.first).c_str(), cmd.Battery[b.first].Battery);
            }
            else
            {
                LOG_RELEASE("    [x] Battery %s", DummyConvertDeviceBatteryType(b.first).c_str());
            }
        }
    }
    else
    {
        LOG_RELEASE("    [x] isProcessed");
    }
}

int Testsmain()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    LOG_RELEASE("Test SonyBaseCmd request:");
    SonyBaseCmd sonyBaseCmd = SonyBaseCmd("SonyBaseCmd");
    TestRequest(
        sonyBaseCmd,
        std::vector<unsigned char>{0x3e, 0x00, 0x3c},
        std::vector<unsigned char>{0x3e, 0x00, 0x3c});

    LOG_RELEASE("Test SonyGetAnc request:");
    SonyGetAnc sonyGetAnc = SonyGetAnc();
    TestRequest(
        sonyGetAnc,
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x66, 0x02, 0x76, 0x3c},
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x02, 0x66, 0x02, 0x77, 0x3c});

    SonyAncState state;
    LOG_RELEASE("Test SonyGetAnc response Anc:");
    state.AncSwitch = SonyAncSwitch::On;
    state.AncFilter = SonyAncFilter::Anc;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 0;
    TestSonyGetAncResponse(
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x67, 0x02, 0x01, 0x02, 0x02, 0x01, 0x00, 0x00, 0x83, 0x3c},
        state);

    LOG_RELEASE("Test SonyGetAnc response Ambient:");
    state.AncSwitch = SonyAncSwitch::On;
    state.AncFilter = SonyAncFilter::Ambient;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 1;
    TestSonyGetAncResponse(
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x67, 0x02, 0x01, 0x02, 0x00, 0x01, 0x00, 0x01, 0x82, 0x3c},
        state);

    LOG_RELEASE("Test SonyGetAnc response Off:");
    state.AncSwitch = SonyAncSwitch::Off;
    state.AncFilter = SonyAncFilter::Ambient;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 1;
    TestSonyGetAncResponse(
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x67, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x81, 0x3c},
        state);

    LOG_RELEASE("Test SonyGetAnc response Wind:");
    state.AncSwitch = SonyAncSwitch::On;
    state.AncFilter = SonyAncFilter::Wind;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 0;
    TestSonyGetAncResponse(
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x08, 0x67, 0x02, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x83, 0x3c},
        state);

    LOG_RELEASE("Test SonyGetCaseBattery request:");
    SonyGetCaseBattery sonyGetCaseBattery = SonyGetCaseBattery();
    TestRequest(
        sonyGetCaseBattery,
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x02, 0x20, 0x3c},
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x02, 0x10, 0x02, 0x21, 0x3c});

    LOG_RELEASE("Test SonyGetCaseBattery response:");

    std::map<DeviceBatteryType, DeviceBatteryData> BatteryCase70;
    DeviceBatteryData battery;
    battery.isCharging = false;
    battery.Status = DeviceBatteryStatus::Cached;
    battery.Battery = 70;
    BatteryCase70[DeviceBatteryType::Case] = battery;

    TestSonyGetCaseBatteryResponse(
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x04, 0x11, 0x02, 0x46, 0x00, 0x6a, 0x3c},
        BatteryCase70);

    LOG_RELEASE("Test SonyGetHeadphonesBattery request:");
    SonyGetHeadphonesBattery sonyGetHeadphonesBattery = SonyGetHeadphonesBattery(SonyProductIds::WF_1000XM3);
    TestRequest(
        sonyGetHeadphonesBattery,
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x01, 0x1f, 0x3c},
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x02, 0x10, 0x01, 0x20, 0x3c});

    LOG_RELEASE("Test SonyGetHeadphonesBatteryL20R50response:");

    std::map<DeviceBatteryType, DeviceBatteryData> BatteryL20R50;
    DeviceBatteryData batteryL;
    batteryL.isCharging = false;
    batteryL.Status = DeviceBatteryStatus::Connected;
    batteryL.Battery = 20;
    BatteryL20R50[DeviceBatteryType::Left] = batteryL;

    DeviceBatteryData batteryR;
    batteryR.isCharging = false;
    batteryR.Status = DeviceBatteryStatus::Connected;
    batteryR.Battery = 50;
    BatteryL20R50[DeviceBatteryType::Right] = batteryR;

    TestSonyGetHeadphonesBatteryResponse(
        SonyProductIds::WF_1000XM3,
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x06, 0x11, 0x01, 0x14, 0x00, 0x32, 0x00, 0x6b, 0x3c},
        BatteryL20R50);

    LOG_RELEASE("Test SonyGetHeadphonesBatteryL80R0 response:");

    std::map<DeviceBatteryType, DeviceBatteryData> BatteryL80R0;
    DeviceBatteryData batteryS;
    batteryS.isCharging = false;
    batteryS.Status = DeviceBatteryStatus::Connected;
    batteryS.Battery = 80;
    BatteryL80R0[DeviceBatteryType::Single] = batteryS;

    TestSonyGetHeadphonesBatteryResponse(
        SonyProductIds::WH_1000XM4,
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x06, 0x11, 0x01, 0x50, 0x00, 0x00, 0x00, 0x75, 0x3c},
        BatteryL80R0);


    LOG_RELEASE("Test SonySetAnc Anc request:");
    state.AncSwitch = SonyAncSwitch::On;
    state.AncFilter = SonyAncFilter::Anc;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 0;

    SonySetAnc sonySetAncAnc = SonySetAnc(state);
    TestRequest(
        sonySetAncAnc,
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x10, 0x02, 0x02, 0x01, 0x00, 0x00, 0x93, 0x3c},
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x10, 0x02, 0x02, 0x01, 0x00, 0x00, 0x94, 0x3c});        


    LOG_RELEASE("Test SonySetAnc Wind request:");
    state.AncSwitch = SonyAncSwitch::On;
    state.AncFilter = SonyAncFilter::Wind;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 0;

    SonySetAnc sonySetAncWind = SonySetAnc(state);
    TestRequest(
        sonySetAncWind,
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x10, 0x02, 0x01, 0x01, 0x00, 0x00, 0x92, 0x3c},
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x10, 0x02, 0x01, 0x01, 0x00, 0x00, 0x93, 0x3c});        
    

    LOG_RELEASE("Test SonySetAnc AmbientVoiceFalseVolume1 Override request:");
    state.AncSwitch = SonyAncSwitch::OnAndSave;
    state.AncFilter = SonyAncFilter::Ambient;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 1;

    SonySetAnc sonySetAncAmbientFalse = SonySetAnc(state);
    TestRequest(
        sonySetAncAmbientFalse,
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x11, 0x02, 0x00, 0x01, 0x00, 0x01, 0x93, 0x3c},
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x11, 0x02, 0x00, 0x01, 0x00, 0x01, 0x94, 0x3c});        


    LOG_RELEASE("Test SonySetAnc Disable request:");
    state.AncSwitch = SonyAncSwitch::Off;
    state.AncFilter = SonyAncFilter::Ambient;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 0;

    SonySetAnc sonySetAncDisable = SonySetAnc(state);
    TestRequest(
        sonySetAncDisable,
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x81, 0x3c},
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x08, 0x68, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x82, 0x3c});        
    
    
    LOG_RELEASE("Test SonyAncWatcher response Anc:");
    state.AncSwitch = SonyAncSwitch::On;
    state.AncFilter = SonyAncFilter::Anc;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 0;
    TestSonyAncWatcher(
        std::vector<unsigned char>{0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x08, 0x69, 0x02, 0x01, 0x02, 0x02, 0x01, 0x00, 0x00, 0x85, 0x3c},
        state);

    LOG_RELEASE("Test SonyAncWatcher response Ambient:");
    state.AncSwitch = SonyAncSwitch::On;
    state.AncFilter = SonyAncFilter::Ambient;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 1;
    TestSonyAncWatcher(
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x08, 0x69, 0x02, 0x01, 0x02, 0x00, 0x01, 0x00, 0x01, 0x85, 0x3c},
        state);

    LOG_RELEASE("Test SonyAncWatcher response Off:");
    state.AncSwitch = SonyAncSwitch::Off;
    state.AncFilter = SonyAncFilter::Ambient;
    state.AmbientVoice = SonyAncFilterAmbientVoice::Off;
    state.Volume = 1;
    TestSonyAncWatcher(
        std::vector<unsigned char>{0x3e, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x08, 0x69, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x84, 0x3c},
        state);
    
    return 0;
}