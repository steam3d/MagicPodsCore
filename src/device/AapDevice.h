#pragma once

#include "../Event.h"
#include "../Device.h"

namespace MagicPodsCore
{
    class AapDevice : public Device
    {
    private:        
        Event<const std::vector<unsigned char>> _onResponseDataRecived{};
        //Add client

    public:
        explicit AapDevice(const sdbus::ObjectPath &objectPath,
                                  const std::map<std::string, sdbus::Variant> &deviceInterface);

        Event<const std::vector<unsigned char>>& GetResponseDataRecived() {
            return _onResponseDataRecived;
        }   

        void SendData(const AapRequest& setter);
    };
}