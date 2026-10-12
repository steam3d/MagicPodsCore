// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/enums/AapStemControlState.h"
#include "sdk/aap/watchers/AapStemControlStateWatcher.h"

namespace MagicPodsCore
{
    class AapStemControlStateCapability : public AapCapability
    {
    private:
        AapStemControlState option{};
        AapStemControlStateWatcher watcher{};
        size_t watcherEventId;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapStemControlStateCapability(AapDevice& device);
        ~AapStemControlStateCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};
    };
}
