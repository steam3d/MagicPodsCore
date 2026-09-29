// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/pbp/enums/PixelBudsModelIds.h"
#include "sdk/pbp/PwRpcPacket.h"
#include "device/structs/DeviceBatteryData.h"
#include "vector"

namespace MagicPodsCore
{
    class PixelBudsBatteryWatcher
    {
    private:
        PixelBudsModelIds model;
        Event<std::vector<DeviceBatteryData>> _batteryChanged{};
        std::vector<DeviceBatteryData> Process_SubscribeRuntimeInfo(const PwRpcPacket &packet);

    public:
        explicit PixelBudsBatteryWatcher(PixelBudsModelIds model) : model(model) {}
        void ProcessResponse(const PwRpcPacket &packet);
        Event<std::vector<DeviceBatteryData>> &GetBatteryChangedEvent()
        {
            return _batteryChanged;
        }
    };
}
