#pragma once

#include "../Event.h"
#include "../Device.h"
#include "../aap/setters/AapRequest.h"
#include "capabilities/aap/AapAncCapability.h"
#include "capabilities/aap/AapBatteryCapability.h"

namespace MagicPodsCore
{
    class AapDevice : public Device
    {
    private:
        Event<const std::vector<unsigned char>> _onResponseDataRecived{};
        void OnResponseDataReceived(std::vector<unsigned char> data) override;
        explicit AapDevice(const sdbus::ObjectPath &objectPath,
                           const std::map<std::string, sdbus::Variant> &deviceInterface);

    public:
        Event<const std::vector<unsigned char>> &GetResponseDataRecived()
        {
            return _onResponseDataRecived;
        }

        void SendData(const AapRequest &setter);

        static std::unique_ptr<AapDevice> Create(const sdbus::ObjectPath &objectPath,
                                                 const std::map<std::string, sdbus::Variant> &deviceInterface);
    };
}