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
    // ---------------------------------------------------------------------------
    // Request
    // 3e0c00000000026602763c
    //    |Body packet          |
    // 0  1  2  3  4  5  6  7  8  9  10
    // s     p              c     cr e
    // 3e 0c 00 00 00 00 02 66 02 76 3c
    //
    // 0 byte:
    // 3e - Packet start (const)
    // 
    // 2 byte:
    // 00 - prefix
    // 01 - prefix
    //
    // 7 byte: command
    //
    // 9 byte: CRC 
    // 
    // 10 byte: 
    // 3c - Packed end (const)    
    // ---------------------------------------------------------------------------
    class SonyGetAnc : public SonyBaseCmd {
    public:
        SonyGetAnc();
        SonyAncState state{};
        void ProcessResponse(const std::vector<unsigned char> &bytes) override;
    protected:
        std::vector<unsigned char>CreatePacketBody(unsigned char prefix) const override;
    };
}
