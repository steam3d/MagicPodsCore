// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "device/structs/DeviceEarDetectionData.h"
#include "sdk/aap/watchers/AapEarDetectionWatcher.h"

namespace MagicPodsCore
{
    class AapEarDetectionStateCapability : public AapCapability
    {
    private:
        DeviceEarDetectionData option;
        AapEarDetectionWatcher watcher{};
        size_t watcherEventId;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapEarDetectionStateCapability(AapDevice& device);
        ~AapEarDetectionStateCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}
