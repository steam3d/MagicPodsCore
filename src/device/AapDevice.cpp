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

    AapDevice::AapDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo) : Device(deviceInfo)
    {
    }

    void AapDevice::SendData(const AapRequest &setter) //TODO: MAKE COMMON CLASS FOR SETTERS
    {
        _client->SendData(setter.Request());
    }

    std::shared_ptr<AapDevice> AapDevice::Create(std::shared_ptr<DBusDeviceInfo> deviceInfo)
    {
        auto device = std::make_shared<AapDevice>(deviceInfo);        
        
        device->capabilities.push_back(std::make_unique<AapBatteryCapability>(device));
        device->capabilities.push_back(std::make_unique<AapAncCapability>(device));

        std::vector<std::vector<unsigned char>> initData;        
        initData.push_back(AapInit{}.Request());
        initData.push_back(AapEnableNotifications{AapNotificationsMode::Unknown1}.Request());      
        if (AapInitExt::IsSupported(deviceInfo->GetProductId()))
            initData.push_back(AapInitExt{}.Request());
        
        //TODO: Add initData to client
        device->_client = Client::CreateL2CAP(deviceInfo->GetAddress(), 0x1001);
        
        device->Init();
        return device;
    }
}