#include "AapDevice.h"
#include "capabilities/aap/AapAncCapability.h"
#include "capabilities/aap/AapBatteryCapability.h"
#include "sdk/aap/setters/AapInit.h"
#include "sdk/aap/setters/AapInitExt.h"
#include "sdk/aap/setters/AapEnableNotifications.h"

namespace MagicPodsCore
{
    void AapDevice::OnResponseDataReceived(const std::vector<unsigned char> &data)
    {
        _onResponseDataRecived.FireEvent(data);
    }

    AapDevice::AapDevice(const sdbus::ObjectPath &objectPath, const std::map<std::string, sdbus::Variant> &deviceInterface)
        : Device(objectPath, deviceInterface)
    {
        capabilities.push_back(std::make_unique<AapBatteryCapability>(*this));
        capabilities.push_back(std::make_unique<AapAncCapability>(*this));
    }

    void AapDevice::SendData(const AapRequest &setter) // TODO MAKE COMMON CLASS FOR SETTERS
    {
        _client->SendData(setter.Request());
    }

    std::unique_ptr<AapDevice> AapDevice::Create(const sdbus::ObjectPath &objectPath, const std::map<std::string, sdbus::Variant> &deviceInterface)
    {
        auto device = new AapDevice(objectPath, deviceInterface);
        
        std::vector<std::vector<unsigned char>> initData;        
        initData.push_back(AapInit{}.Request());
        initData.push_back(AapEnableNotifications{AapNotificationsMode::Unknown1}.Request());      
        if (AapInitExt::IsSupported(device->_productId))
        initData.push_back(AapInitExt{}.Request());
        
        //TODO:Add initData to client
        device->_client = Client::CreateL2CAP(device->_address, 0x1001);
        
        device->Init();
        return std::unique_ptr<AapDevice>(device);
    }
}