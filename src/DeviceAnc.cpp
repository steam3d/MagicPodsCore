#include "DeviceAnc.h"

namespace MagicPodsCore {

    void DeviceAnc::ClearAnc(){
        _mode = DeviceAncMode::NotAvailable;
    }

    void DeviceAnc::UpdateFromAppleAnc(AncMode aapMode){
        DeviceAncMode mode = DeviceAnc::AncModeToDeviceAncMode(aapMode);
        printf("Trying update mode\n");
        if (mode != _mode){
            _mode = mode;
            printf("Mode updated\n");
            _modeChanged.FireEvent(_mode);
        }
    }

    DeviceAncMode DeviceAnc::AncModeToDeviceAncMode(AncMode aapMode){
        DeviceAncMode mode;
        switch (aapMode)
        {
            case AncMode::Off:
                mode = DeviceAncMode::Off;
                break;        
            case AncMode::Tra:
                mode = DeviceAncMode::Tra;
                break;    
            case AncMode::On:
                mode = DeviceAncMode::On;
                break;    
            default:
                mode= DeviceAncMode::NotAvailable;
                break;
        }
        
        return mode;
    }

    AncMode DeviceAnc::DeviceAncModeAncModeTo(DeviceAncMode mode){
        AncMode aapMode;
        switch (mode)
        {
            case DeviceAncMode::Off:
                aapMode = AncMode::Off;
                break;        
            case DeviceAncMode::Tra:
                aapMode = AncMode::Tra;
                break;    
            case DeviceAncMode::On:
                aapMode = AncMode::On;
                break;    
            default:
                aapMode = AncMode::Off;
                break;
        }
        
        return aapMode;
    }
}