#pragma once

namespace MagicPodsCore {

    enum class GalaxyBudsLegacyEarDetectionState: unsigned char{
        None = 0,
        R = 1,
        L = 16,
        Both = 17
    };

    static bool isValidGalaxyBudsLegacyEarDetectionStateType(unsigned char value){
        switch (static_cast<GalaxyBudsLegacyEarDetectionState>(value)) {
            case GalaxyBudsLegacyEarDetectionState::None:
            case GalaxyBudsLegacyEarDetectionState::R:
            case GalaxyBudsLegacyEarDetectionState::L:
            case GalaxyBudsLegacyEarDetectionState::Both:
                return true;
            default:
                return false;
        }
    }
}