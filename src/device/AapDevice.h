#pragma once

#include "../Event.h"
#include "../Device.h"
#include "../aap/setters/AapRequest.h"
class AapAncCapability;
class AapBatteryCapability;


namespace MagicPodsCore
{

    class AapDevice : public Device
    {
    private:
        Event<const std::vector<unsigned char>> _onResponseDataRecived{};
        void OnResponseDataReceived(const std::vector<unsigned char> &data) override;

    public:
        explicit AapDevice(const sdbus::ObjectPath &objectPath,
                           const std::map<std::string, sdbus::Variant> &deviceInterface);
        Event<const std::vector<unsigned char>> &GetResponseDataRecived()
        {
            return _onResponseDataRecived;
        }

        void SendData(const AapRequest &setter);

        static std::unique_ptr<AapDevice> Create(const sdbus::ObjectPath &objectPath,
                                                 const std::map<std::string, sdbus::Variant> &deviceInterface);
    };
}