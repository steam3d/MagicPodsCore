#include "AapDevice.h"
#include "capabilities/aap/AapAncCapability.h"
#include "capabilities/aap/AapBatteryCapability.h"
#include "../aap/setters/AapInit.h"
#include "../aap/setters/AapEnableNotifications.h"

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
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        device->SendData(AapInit{});
        std::this_thread::sleep_for(std::chrono::seconds(1));
        device->SendData(AapEnableNotifications{AapNotificationsMode::Unknown1});

        return std::unique_ptr<AapDevice>(device);
    }
}