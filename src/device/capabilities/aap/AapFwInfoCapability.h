// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/structs/AapFwInfo.h"
#include "sdk/aap/watchers/AapFwInfoWatcher.h"

namespace MagicPodsCore
{
    class AapFwInfoCapability : public AapCapability
    {
    private:
        AapFwInfo option{};
        AapFwInfoWatcher watcher{};
        size_t watcherEventId{};

        static bool AreEqual(const AapFwInfo& first, const AapFwInfo& second);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapFwInfoCapability(AapDevice& device);
        ~AapFwInfoCapability() override;
        void SetFromJson(const nlohmann::json& json) override {};
    };
}
