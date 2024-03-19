#pragma once
#include "SonyBaseCmd.h"
#include "SonyProductIds.h"
#include "../DeviceBattery.h"

namespace MagicPodsCore
{
    class SonyGetHeadphonesBattery : public SonyBaseCmd {
    public:
        std::map<DeviceBatteryType, DeviceBatteryData> Battery;
        SonyGetHeadphonesBattery(SonyProductIds model);
        void ProcessResponse(const std::vector<unsigned char> &bytes) override;
    private:
        SonyProductIds model; 
    protected:
        std::vector<unsigned char>CreatePacketBody(unsigned char prefix) const override;
    };
}
