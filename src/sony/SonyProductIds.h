#pragma once

// Modalias: bluetooth:v{VendorIds}p{AppleProductIds}dB087
enum class SonyProductIds : unsigned short {
    WH_1000XM4 = 0x0d58,
    WF_1000XM3 = 0x0ce0,
};

static const SonyProductIds AllSonyProductIds[] = {
    SonyProductIds::WH_1000XM4,
    SonyProductIds::WF_1000XM3,
};