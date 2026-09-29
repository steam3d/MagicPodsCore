// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "PixelBudsCapability.h"
#include "sdk/pbp/watchers/PixelBudsAncWatcher.h"
#include "device/enums/DeviceAncModes.h"

namespace MagicPodsCore
{
    class PixelBudsAncCapability : public PixelBudsCapability
    {
    private:
        DeviceAncModes option;
        PixelBudsAncWatcher watcher;
        size_t watcherAncChangedEventId;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const PwRpcPacket &packet);

    public:
        static PixelBudsAncState DeviceAncModesToPixelBudsAncState(DeviceAncModes mode);
        static DeviceAncModes PixelBudsAncStateToDeviceAncModes(PixelBudsAncState state);

        explicit PixelBudsAncCapability(PixelBudsDevice& device);
        ~PixelBudsAncCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}
