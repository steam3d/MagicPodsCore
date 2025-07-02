// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapToneVolumeWatcher.h"

namespace MagicPodsCore
{
    class AapToneVolumeCapability : public AapCapability
    {
    private:
        unsigned char option;        
        AapToneVolumeWatcher watcher{};
        size_t watcherEventId;
        unsigned char ConvertToRange15_100(unsigned char value);        
        unsigned char ConvertToRange15_125(unsigned char value);        
        
    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapToneVolumeCapability(AapDevice& device);
        ~AapToneVolumeCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}