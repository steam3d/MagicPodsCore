// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapAncOffMode.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009003401000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 34 01 00 00 00 Enabled
    // 04 00 04 00 09 00 34 02 00 00 00 Disabled
    //
    // 7 byte:
    // 01 - Enabled
    // 02 - Disabled
    // Controls whether Off is available as a listening mode
    // ---------------------------------------------------------------------------
    class AapSetAncOffMode : public AapRequest
    {
    private:
        AapAncOffMode _mode;

    public:
        explicit AapSetAncOffMode(AapAncOffMode mode);
        std::vector<unsigned char> Request() const override;
    };
}
