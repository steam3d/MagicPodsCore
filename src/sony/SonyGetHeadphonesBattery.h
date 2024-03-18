#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "SonyBaseCmd.h"
#include "../DeviceBattery.h"

namespace MagicPodsCore
{
    class SonyGetHeadphonesBattery : public SonyBaseCmd {
    public:
        std::map<DeviceBatteryType, DeviceBatteryData> Battery;
        SonyGetHeadphonesBattery();
        void ProcessResponse(const std::vector<unsigned char> &bytes) override;
    protected:
        std::vector<unsigned char>CreatePacketBody(unsigned char prefix) const override;
    };
}
