// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapDigitalCrownMode.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001c01000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 1c 01 00 00 00 Back to front
    // 04 00 04 00 09 00 1c 02 00 00 00 Front to back
    //
    // 7 byte:
    // 01 - Back to front
    // 02 - Front to back
    // Selects the Digital Crown rotation direction used to adjust volume
    // ---------------------------------------------------------------------------
    class AapSetDigitalCrown : public AapRequest
    {
    private:
        AapDigitalCrownMode _mode;

    public:
        explicit AapSetDigitalCrown(AapDigitalCrownMode mode);
        std::vector<unsigned char> Request() const override;
    };
}
