#include "GalaxyBudsDevice.h"

namespace MagicPodsCore
{
    GalaxyBudsDevice::GalaxyBudsDevice(const sdbus::ObjectPath &objectPath,
                                       const std::map<std::string, sdbus::Variant> &deviceInterface,                                       
                                       unsigned short model)
        : Device(objectPath, deviceInterface),
          _customProductId(model)
    {       
        //MUST BE INITIALIZED BEFORE USE
        _client = MagicPodsCore::Client::CreateRFCOMM(_address,
            GalaxyBudsHelper::GetServiceGuid(static_cast<GalaxyBudsModelIds>(model)));
    }

    void GalaxyBudsDevice::SendData(const GalaxyBudsSetAnc& setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        // TODO SEND DATA TO GalaxyBudsClient
    }
}
