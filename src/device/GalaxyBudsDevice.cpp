#include "GalaxyBudsDevice.h"

#include "sdk/sgb/GalaxyBudsHelper.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "capabilities/sgb/GalaxyBudsAncCapability.h"
#include "capabilities/sgb/GalaxyBudsBatteryCapability.h"

namespace MagicPodsCore
{
    void GalaxyBudsDevice::OnResponseDataReceived(const std::vector<unsigned char> &data)
    {
        std::optional<GalaxyBudsResponseData> optionalData = _packet.Extract(data);
        if (optionalData.has_value())
            _ResponseDataRecived.FireEvent(optionalData.value());
    }

    GalaxyBudsDevice::GalaxyBudsDevice(const sdbus::ObjectPath &objectPath,
                                       const std::map<std::string, sdbus::Variant> &deviceInterface,
                                       unsigned short model)
        : Device(objectPath, deviceInterface),
          _customProductId(model),
          _packet(static_cast<GalaxyBudsModelIds>(model)) {}

    void GalaxyBudsDevice::SendData(const GalaxyBudsSetAnc &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        _client->SendData(_packet.Encode(setter.Id, setter.Payload));
    }

    std::shared_ptr<GalaxyBudsDevice> GalaxyBudsDevice::Create(const sdbus::ObjectPath &objectPath,
                                                        const std::map<std::string, sdbus::Variant> &deviceInterface,
                                                        unsigned short model)
    {
        auto device = std::make_shared<GalaxyBudsDevice>(objectPath, deviceInterface, model);  

        device->capabilities.push_back(std::make_unique<GalaxyBudsBatteryCapability>(device));
        device->capabilities.push_back(std::make_unique<GalaxyBudsAncCapability>(device));
        
        device->_client = Client::CreateRFCOMM(device->_address, GalaxyBudsHelper::GetServiceGuid(static_cast<GalaxyBudsModelIds>(model)));
        device->Init();
        return device;
    }
}
