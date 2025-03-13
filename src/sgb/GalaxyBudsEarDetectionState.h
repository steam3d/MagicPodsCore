#pragma once

namespace MagicPodsCore {

    enum class GalaxyBudsEarDetectionState: unsigned char{
        Disconnected = 0,        
        Wearing = 1,        
        Idle = 2,        
        Case = 3,        
        ClosedCase = 4,
    };

    static bool isValidGalaxyBudsEarDetectionStateType(unsigned char value){
        switch (static_cast<GalaxyBudsEarDetectionState>(value)) {
            case GalaxyBudsEarDetectionState::Disconnected:
            case GalaxyBudsEarDetectionState::Wearing:
            case GalaxyBudsEarDetectionState::Idle:
            case GalaxyBudsEarDetectionState::Case:                
            case GalaxyBudsEarDetectionState::ClosedCase:                
                return true;
            default:
                return false;
        }
    }
}