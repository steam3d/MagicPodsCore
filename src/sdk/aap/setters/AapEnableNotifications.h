// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapNotificationsMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 040004000f00ffffffff
    //
    // 0  1  2  3  4  5  6  7  8  9
    //                         m
    // 04 00 04 00 0f 00 ff ff ff ff Unknown1
    // 04 00 04 00 0f 00 ff ff ef ff Unknown2
    //
    // 8 byte:
    // ff - Unknown1
    // ef - Unknown2
    // The meaning of the individual notification filter bits is unknown
    // Enables status notifications from the headphones
    // ---------------------------------------------------------------------------
    class AapEnableNotifications : public AapRequest
    {
    private:
        AapNotificationsMode _mode{};

    public:
        AapEnableNotifications(AapNotificationsMode mode);
        std::vector<unsigned char> Request() const override;
    };
}
