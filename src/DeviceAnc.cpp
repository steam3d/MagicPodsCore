#include "DeviceAnc.h"

#include "Logger.h"

namespace MagicPodsCore {

    void DeviceAnc::ClearAnc(){
        _mode = DeviceAncMode::NotAvailable;
    }

    void DeviceAnc::UpdateFromAppleAnc(AapAncMode aapMode){
        DeviceAncMode mode = DeviceAnc::AncModeToDeviceAncMode(aapMode);
        LOG_RELEASE("Trying update anc mode %u -> %u", static_cast<unsigned char>(_mode), static_cast<unsigned char>(mode));
        if (mode != _mode){
            _mode = mode;
            LOG_RELEASE("Updated anc mode: %u",  static_cast<unsigned char>(_mode));
            _modeChanged.FireEvent(_mode);
        }
    }

    DeviceAncMode DeviceAnc::AncModeToDeviceAncMode(AapAncMode aapMode){
        DeviceAncMode mode;
        switch (aapMode)
        {
            case AapAncMode::Off:
                mode = DeviceAncMode::Off;
                break;        
            case AapAncMode::Transparency:
                mode = DeviceAncMode::Tra;
                break;    
            case AapAncMode::Anc:
                mode = DeviceAncMode::On;
                break;    
            default:
                mode= DeviceAncMode::NotAvailable;
                break;
        }
        
        return mode;
    }

    AapAncMode DeviceAnc::DeviceAncModeAncModeTo(DeviceAncMode mode){
        AapAncMode aapMode;
        switch (mode)
        {
            case DeviceAncMode::Off:
                aapMode = AapAncMode::Off;
                break;        
            case DeviceAncMode::Tra:
                aapMode = AapAncMode::Transparency;
                break;    
            case DeviceAncMode::On:
                aapMode = AapAncMode::Anc;
                break;    
            default:
                aapMode = AapAncMode::Off;
                break;
        }
        
        return aapMode;
    }
}