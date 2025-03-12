#pragma once

namespace MagicPodsCore {

    enum class GalaxyBudsMsgIds: unsigned char{
        UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT = 66,
        STATUS_UPDATED = 96,
        EXTENDED_STATUS_UPDATED = 97,
        NOISE_CONTROLS_UPDATE = 119,
        NOISE_CONTROLS = 120,
    };

    static bool isValidGalaxyBudsMsgIdsType(unsigned char value){
        switch (static_cast<GalaxyBudsMsgIds>(value)) {
            case GalaxyBudsMsgIds::UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT:
            case GalaxyBudsMsgIds::STATUS_UPDATED:
            case GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED:
            case GalaxyBudsMsgIds::NOISE_CONTROLS_UPDATE:
            case GalaxyBudsMsgIds::NOISE_CONTROLS:            
                return true;
            default:
                return false;
        }
    }
}