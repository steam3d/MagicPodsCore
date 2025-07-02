// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>

namespace MagicPodsCore
{
    // Byte 4 indicates the packet type
    enum class AapConversationAwarenessState : unsigned char
    {
        Unknown = 0x00,
        //0x02 possibly speaking
        //0x03, ... unknown
        //0x08 possible stop speaking
        StartSpeaking = 0x01,
        StopSpeaking1 = 0x04,
        StopSpeaking2 = 0x06,
        StopSpeaking3 = 0x08,
        StopSpeaking4 = 0x09,
    };

    static bool isValidAapConversationAwarenessState(unsigned char value){
        switch (static_cast<AapConversationAwarenessState>(value)) {
            case AapConversationAwarenessState::Unknown:
            case AapConversationAwarenessState::StartSpeaking:
            case AapConversationAwarenessState::StopSpeaking1:
            case AapConversationAwarenessState::StopSpeaking2:
            case AapConversationAwarenessState::StopSpeaking3:
            case AapConversationAwarenessState::StopSpeaking4:
                return true;
            default:
                return false;
        }
    }

    static std::string AapConversationAwarenessStateToString(AapConversationAwarenessState value)
    {
        switch (value)
        {
        case AapConversationAwarenessState::Unknown:
            return "Unknown";
        case AapConversationAwarenessState::StartSpeaking:
            return "StartSpeaking";
        case AapConversationAwarenessState::StopSpeaking1:
            return "StopSpeaking1";
        case AapConversationAwarenessState::StopSpeaking2:
            return "StopSpeaking2";
        case AapConversationAwarenessState::StopSpeaking3:
            return "StopSpeaking3";
        case AapConversationAwarenessState::StopSpeaking4:
            return "StopSpeaking4";            
        default:
            return "Unknown value";
        }
    }

}