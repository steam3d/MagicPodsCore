#include "AapDevice.h"

namespace MagicPodsCore
{
    AapDevice::AapDevice(const sdbus::ObjectPath &objectPath, const std::map<std::string, sdbus::Variant> &deviceInterface)
        : Device(objectPath, deviceInterface)
    {
        
    }

    void AapDevice::SendData(const AapRequest& setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        // TODO SEND DATA TO GalaxyBudsClient
    }
}