// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapNotificationsMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040030000500
    //
    // Requests the IRK and ENC keys for decrypting private BLE advertisements.
    // Response packet type: 0x31.
    // ---------------------------------------------------------------------------
    class AapPrivateKeys : public AapRequest
    {

    public:
        AapPrivateKeys();
        std::vector<unsigned char> Request() const override;
    };
}
