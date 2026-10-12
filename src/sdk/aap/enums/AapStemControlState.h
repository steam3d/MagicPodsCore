// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    // Byte 7 contains the stem control bit mask
    // RESP: Byte 6 indicates the press type
    enum class AapStemControlPress : unsigned char
    {
        SinglePress = 0x05,
        DoublePress = 0x06,
        TriplePress = 0x07,
        LongPress = 0x08,
    };

    // RESP: Byte 7 indicates the AirPod
    enum class AapStemControlBud : unsigned char
    {
        Left = 0x01,
        Right = 0x02,
    };

    struct AapStemControlState
    {
        AapStemControlPress Press;
        AapStemControlBud Bud;
    };

    static bool isValidAapStemControlPress(unsigned char value)
    {
        switch (static_cast<AapStemControlPress>(value))
        {
        case AapStemControlPress::SinglePress:
        case AapStemControlPress::DoublePress:
        case AapStemControlPress::TriplePress:
        case AapStemControlPress::LongPress:
            return true;
        default:
            return false;
        }
    }

    static bool isValidAapStemControlBud(unsigned char value)
    {
        switch (static_cast<AapStemControlBud>(value))
        {
        case AapStemControlBud::Left:
        case AapStemControlBud::Right:
            return true;
        default:
            return false;
        }
    }

    static std::string AapStemControlPressToString(AapStemControlPress value)
    {
        switch (value)
        {
        case AapStemControlPress::SinglePress:
            return "SinglePress";
        case AapStemControlPress::DoublePress:
            return "DoublePress";
        case AapStemControlPress::TriplePress:
            return "TriplePress";
        case AapStemControlPress::LongPress:
            return "LongPress";
        default:
            return "Unknown value";
        }
    }

    static std::string AapStemControlBudToString(AapStemControlBud value)
    {
        switch (value)
        {
        case AapStemControlBud::Left:
            return "Left";
        case AapStemControlBud::Right:
            return "Right";
        default:
            return "Unknown value";
        }
    }
}
