#pragma once

#include "Event.h"
#include "aap/AapClient.h"

namespace MagicPodsCore {
    
    // Hardcoded for UI
    enum class DeviceAncMode {    
        NotAvailable = 0x00,
        Off = 0x01,
        Tra = 0x02,
        On = 0x03,
    };

    class DeviceAnc {
    private:
        Event<DeviceAncMode> _modeChanged{};
        DeviceAncMode _mode{};
    
    public:
        Event<DeviceAncMode>& GetAncChangedEvent() {
            return _modeChanged;
        }
        
        DeviceAncMode GetAncStatus() const {
            return _mode;
        }

        void UpdateFromAppleAnc(AncMode aapMode);

        void ClearAnc();

        AncMode DeviceAncModeAncModeTo(DeviceAncMode mode);

    private:
        DeviceAncMode AncModeToDeviceAncMode(AncMode aapMode);
    };
}