// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapEarDetectionWatcher.h"
#include "device/structs/DeviceEarDetectionData.h"

namespace MagicPodsCore
{
    class AapEarDetectionCapability : public AapCapability
    {
    private:
        DeviceEarDetectionData option;
        AapEarDetectionWatcher watcher{};
        size_t watcherEventId;
        void TriggerMediaControl(DeviceEarStatus previousStatus);
        void TriggerAudioOutputSwitch();
        void TriggerVolumeControl(DeviceEarStatus previousStatus);
        bool LoadSetting();
        void SaveSetting(bool b);
        bool LoadSwitchToSpeakersSetting();
        void SaveSwitchToSpeakersSetting(bool b);
        int LoadLowerVolumeSetting();
        void SaveLowerVolumeSetting(int value);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapEarDetectionCapability(AapDevice& device);
        ~AapEarDetectionCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}
