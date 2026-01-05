// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "Device.h"

namespace MagicPodsCore
{

    class BhfDevice : public Device
    {

    private:
    void OnResponseDataReceived(const std::vector<unsigned char> &data) override;

    public:
        explicit BhfDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo);
        static std::unique_ptr<BhfDevice> Create(std::shared_ptr<DBusDeviceInfo> deviceInfo);        
    };
}