// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/structs/AapCustomEq.h"
#include "sdk/aap/watchers/AapCustomEqWatcher.h"

namespace MagicPodsCore
{
    class AapCustomEqCapability : public AapCapability
    {
    private:
        AapCustomEq option{};
        AapCustomEqWatcher watcher{};
        size_t watcherEventId{};

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapCustomEqCapability(AapDevice& device);
        ~AapCustomEqCapability() override;
        void SetFromJson(const nlohmann::json& json) override;
    };
}
