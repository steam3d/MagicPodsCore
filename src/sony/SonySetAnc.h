#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "SonyBaseCmd.h"
#include "SonyStructs.h"

namespace MagicPodsCore
{   
    class SonySetAnc : public SonyBaseCmd {
    public:
        SonySetAnc(SonyAncState state);
        SonyAncState state{};
        void ProcessResponse(const std::vector<unsigned char> &bytes) override;
    protected:
        std::vector<unsigned char>CreatePacketBody(unsigned char prefix) const override;
    };
}
