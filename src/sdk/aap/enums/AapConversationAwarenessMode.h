// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 7 indicates the packet type
    enum class AapConversationAwarenessMode : unsigned char
    {
        On = 0x01,
        Off = 0x02,
    };

    static bool isValidAapConversationAwarenessMode(unsigned char value){
        switch (static_cast<AapConversationAwarenessMode>(value)) {
            case AapConversationAwarenessMode::On:
            case AapConversationAwarenessMode::Off:
                return true;
            default:
                return false;
        }
    }

    static std::string AapConversationAwarenessModeToString(AapConversationAwarenessMode value)
    {
        switch (value)
        {
        case AapConversationAwarenessMode::On:
            return "On";
        case AapConversationAwarenessMode::Off:
            return "Off";
        default:
            return "Unknown value";
        }
    }

}