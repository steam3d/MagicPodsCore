#include "AapEnableNotifications.h";

namespace MagicPodsCore
{
    AapEnableNotifications::AapEnableNotifications(AapNotificationsMode mode) : AapRequest{"AapEnableNotifications"}, _mode{mode}
    {
    }

    std::vector<unsigned char> AapEnableNotifications::Request() const
    {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(AapCmd::Notifications), 0x00, 0xff, 0xff, static_cast<unsigned char>(_mode), 0xff};
    }

}