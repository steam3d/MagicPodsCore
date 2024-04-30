#pragma once
#include "SonyBaseCmd.h"
#include "../DeviceBattery.h"

namespace MagicPodsCore
{
    class SonyGetCaseBattery : public SonyBaseCmd {
    public:
        std::map<DeviceBatteryType, DeviceBatteryData> Battery;
        SonyGetCaseBattery();
        void ProcessResponse(const std::vector<unsigned char> &bytes) override;
    protected:
        std::vector<unsigned char>CreatePacketBody(unsigned char prefix) const override;
    };
}
