#pragma once

#include "sdk/aap/enums/AapAncMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
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
    // Request and response the same.
    // Headphones send the same packet after change anc mode.
    // ---------------------------------------------------------------------------
    class AapSetAnc : public AapRequest
    {
    private:
        AapAncMode _mode;

    public:
        AapSetAnc(AapAncMode mode);
        std::vector<unsigned char> Request() const override;
        static std::vector<AapAncMode> GetAncModesFor(unsigned short model);
    };
}