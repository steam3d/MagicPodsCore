// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapEndCallMode : unsigned char
    {
        DoublePress = 0x02,
        SinglePress = 0x03,
    };

    static bool isValidAapEndCallMode(unsigned char value){
        switch (static_cast<AapEndCallMode>(value)) {
            case AapEndCallMode::DoublePress:
            case AapEndCallMode::SinglePress:
                return true;
            default:
                return false;
        }
    }

    static std::string AapEndCallModeToString(AapEndCallMode value)
    {
        switch (value)
        {
        case AapEndCallMode::DoublePress:
            return "DoublePress";
        case AapEndCallMode::SinglePress:
            return "SinglePress";
        default:
            return "Unknown value";
        }
    }

    // Byte 7 indicates the packet type
    enum class AapMuteMicrophoneMode : unsigned char
    {
        DoublePress = 0x22,
        SinglePress = 0x23,
    };

    static bool isValidAapMuteMicrophoneMode(unsigned char value){
        switch (static_cast<AapMuteMicrophoneMode>(value)) {
            case AapMuteMicrophoneMode::DoublePress:
            case AapMuteMicrophoneMode::SinglePress:
                return true;
            default:
                return false;
        }
    }

    static std::string AapMuteMicrophoneModeToString(AapMuteMicrophoneMode value)
    {
        switch (value)
        {
        case AapMuteMicrophoneMode::DoublePress:
            return "DoublePress";
        case AapMuteMicrophoneMode::SinglePress:
            return "SinglePress";
        default:
            return "Unknown value";
        }
    }

    struct AapMuteMicrophoneEndCallMode
    {
        AapEndCallMode endCallMode;
        AapMuteMicrophoneMode muteMicrophoneMode;
    };

}