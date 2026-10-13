// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 040004000900390b000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 39 0b 00 00 00 Single, double, and long press
    //
    // 7 byte:
    // 01 - Single press
    // 02 - Double press
    // 04 - Triple press
    // 08 - Long press
    // Bitmask selects stem gestures handled by the connected device
    // Replaces the headphones' default behavior for each selected gesture
    // ---------------------------------------------------------------------------
    class AapSetStemControl : public AapRequest
    {
    private:
        unsigned char _modes;

    public:
        explicit AapSetStemControl(unsigned char modes);
        std::vector<unsigned char> Request() const override;
    };
}
