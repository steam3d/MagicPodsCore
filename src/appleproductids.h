#pragma once

// Modalias: bluetooth:v{VendorIds}p{AppleProductIds}dB087
enum class AppleProductIds: unsigned short{
    airpods1 = 0x2002,
    airpods2 = 0x200f, 
    airpods3 = 0x2013, 
    airpodspro = 0x200e,
    airpodspro2 = 0x2014,
    airpodsprousbc = 0x2024,
    powerbeatspro = 0x200b,
    airpodsmax = 0x200a,
    beatsSolo3 = 0x2006,
    beatssolopro = 0x200c,
    beatsstudio3 = 0x2009,
    beatsx = 0x2005,
    beatsflex = 0x2010,
    powerbeats3 = 0x2003,
    beatsfitpro = 0x2012,
    beatsstudiobuds = 0x2011,
    beatsstudiobudsplus = 0x2016,
    powerbeats4 = 0x200d,
};

static const AppleProductIds AllAppleProductIds[] = {
    AppleProductIds::airpods1,
    AppleProductIds::airpods2,
    AppleProductIds::airpods3,
    AppleProductIds::airpodspro,
    AppleProductIds::airpodspro2,
    AppleProductIds::airpodsprousbc,
    AppleProductIds::powerbeatspro,
    AppleProductIds::airpodsmax,
    AppleProductIds::beatsSolo3,
    AppleProductIds::beatssolopro,
    AppleProductIds::beatsstudio3,
    AppleProductIds::beatsx,
    AppleProductIds::beatsflex,
    AppleProductIds::powerbeats3,
    AppleProductIds::beatsfitpro,
    AppleProductIds::beatsstudiobuds,
    AppleProductIds::beatsstudiobudsplus,
    AppleProductIds::powerbeats4
};