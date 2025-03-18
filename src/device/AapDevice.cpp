#include "AapDevice.h"

namespace MagicPodsCore
{
    void AapDevice::OnResponseDataReceived(std::vector<unsigned char> data)
    {
        _onResponseDataRecived.FireEvent(data);
    }

    AapDevice::AapDevice(const sdbus::ObjectPath &objectPath, const std::map<std::string, sdbus::Variant> &deviceInterface)
        : Device(objectPath, deviceInterface)
    {
        capabilities.push_back(AapBatteryCapability(*this));
        capabilities.push_back(AapAncCapability(*this));
    }

    void AapDevice::SendData(const AapRequest &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        _client->SendData(setter.Request());
    }

    std::unique_ptr<AapDevice> AapDevice::Create(const sdbus::ObjectPath &objectPath, const std::map<std::string, sdbus::Variant> &deviceInterface)
    {
        auto device = new AapDevice(objectPath, deviceInterface);
        device->_client = Client::CreateL2CAP(device->_address, 0x1001);
        device->Init();
        return std::unique_ptr<AapDevice>(device);
    }
}