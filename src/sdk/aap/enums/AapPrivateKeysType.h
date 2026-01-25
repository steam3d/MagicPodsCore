// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    // Byte 6 indicates the anc mode
    enum class AapPrivateKeysType
    {
        Irk = 0x01,
        Enc = 0x04,
    };

    static bool isValidAapPrivateKeysType(unsigned char value){
        switch (static_cast<AapPrivateKeysType>(value)) {
            case AapPrivateKeysType::Irk:
            case AapPrivateKeysType::Enc:
                return true;
            default:
                return false;
        }
    }


    static std::string AapPrivateKeysTypeToString(AapPrivateKeysType value)
    {
        switch (value)
        {
        case AapPrivateKeysType::Irk:
            return "Irk key";
        case AapPrivateKeysType::Enc:
            return "Encryption key";
        default:
            return "Unknown value";
        }
    }
}