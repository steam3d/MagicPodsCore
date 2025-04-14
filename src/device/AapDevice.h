// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "Device.h"
#include "sdk/aap/setters/AapRequest.h"

namespace MagicPodsCore
{

    class AapDevice : public Device
    {
    private:
        Event<const std::vector<unsigned char>> _onResponseDataRecived{};
        void OnResponseDataReceived(const std::vector<unsigned char> &data) override;

    public:
        explicit AapDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo);
        Event<const std::vector<unsigned char>> &GetResponseDataRecived()
        {
            return _onResponseDataRecived;
        }

        void SendData(const AapRequest &setter);

        static std::unique_ptr<AapDevice> Create(std::shared_ptr<DBusDeviceInfo> deviceInfo);
    };
}