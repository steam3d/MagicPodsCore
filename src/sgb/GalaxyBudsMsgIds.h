#pragma once

namespace MagicPodsCore {

    enum class GalaxyBudsMsgIds: unsigned char{
        UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT = 66,
        STATUS_UPDATED = 96,
        EXTENDED_STATUS_UPDATED = 97,
        NOISE_CONTROLS_UPDATE = 119,
        NOISE_CONTROLS = 120,
    };
}