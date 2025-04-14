// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapNotificationsMode.h"
#include "AapRequest.h"

namespace MagicPodsCore
{
    class AapEnableNotifications : public AapRequest
    {
    private:
        AapNotificationsMode _mode{};

    public:
        AapEnableNotifications(AapNotificationsMode mode);
        std::vector<unsigned char> Request() const override;
    };
}