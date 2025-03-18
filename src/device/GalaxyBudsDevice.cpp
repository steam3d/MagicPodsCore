#include "GalaxyBudsDevice.h"

namespace MagicPodsCore
{
    void GalaxyBudsDevice::OnResponseDataReceived(std::vector<unsigned char> data)
    {
        std::optional<GalaxyBudsResponseData> optionalData = _packet.Extract(data);
        if (optionalData.has_value())
            _onGalaxyBudsResponseDataRecived.FireEvent(optionalData.value());
    }

    GalaxyBudsDevice::GalaxyBudsDevice(const sdbus::ObjectPath &objectPath,
                                       const std::map<std::string, sdbus::Variant> &deviceInterface,
                                       unsigned short model)
        : Device(objectPath, deviceInterface),
          _customProductId(model),
          _packet(static_cast<GalaxyBudsModelIds>(model))
    {
        capabilities.push_back(GalaxyBudsBatteryCapability(*this));
        capabilities.push_back(GalaxyBudsAncCapability(*this));
    }

    void GalaxyBudsDevice::SendData(const GalaxyBudsSetAnc &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        _client->SendData(_packet.Encode(setter.Id, setter.Payload));
    }

    std::unique_ptr<GalaxyBudsDevice> GalaxyBudsDevice::Create(const sdbus::ObjectPath &objectPath,
                                                        const std::map<std::string, sdbus::Variant> &deviceInterface,
                                                        unsigned short model)
    {
        auto device = new GalaxyBudsDevice(objectPath, deviceInterface, model);
        device->_client = Client::CreateRFCOMM(device->_address, GalaxyBudsHelper::GetServiceGuid(static_cast<GalaxyBudsModelIds>(model)));
        device->Init();
        return std::unique_ptr<GalaxyBudsDevice>(device);
    }
}
