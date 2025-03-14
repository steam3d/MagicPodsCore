#pragma once

#include "../enums/AapCmd.h"
#include "../enums/AapNotificationsMode.h"
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