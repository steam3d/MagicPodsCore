// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "audio/VirtualMicrophone.h"
#include "sdk/aap/watchers/AapConversationAwarenessWatcher.h"
#include "sdk/aap/watchers/AapMicrophoneWatcher.h"
#include "settings/SettingsService.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace MagicPodsCore
{
    class AapHighResolutionMicrophoneCapability : public AapCapability
    {
    private:
        static constexpr size_t AudioQueueCapacity = 256;
        static constexpr std::chrono::milliseconds MonitorInterval{400};
        static constexpr std::chrono::milliseconds A2dpResetDelay{800};
        static constexpr std::chrono::milliseconds StallTimeout{2000};
        static constexpr float LevelRelease = 0.85f;

        bool _enabled{false};
        bool _useAutomaticGainControl{true};
        bool _pauseConversationAwareness{true};
        bool _resetA2dp{true};

        std::atomic<bool> _active{false};
        std::atomic<float> _level{0.0f};
        std::atomic<bool> _stopMonitor{false};
        std::atomic<bool> _monitorRunning{false};
        std::atomic<bool> _captureActive{false};
        std::atomic<bool> _outputBroken{false};
        std::atomic<int64_t> _lastAudioPacketTime{0};
        std::atomic<int64_t> _lastLevelNotificationTime{0};
        std::atomic<int> _conversationAwarenessState{-1};

        std::string _application{};
        mutable std::mutex _stateMutex{};

        std::thread _monitorThread{};
        std::thread _decodeThread{};
        std::mutex _monitorLifecycleMutex{};
        std::mutex _monitorMutex{};
        std::condition_variable _monitorCondition{};

        std::deque<AapMicrophoneWatcher::AudioFrames> _audioQueue{};
        std::mutex _audioQueueMutex{};
        std::condition_variable _audioQueueCondition{};

        std::unique_ptr<VirtualMicrophone> _virtualMicrophone{};
        std::shared_ptr<SettingsService> _settingsService{};
        bool _restoreConversationAwareness{false};

        AapConversationAwarenessWatcher _conversationAwarenessWatcher{};
        AapMicrophoneWatcher _microphoneWatcher{};
        size_t _conversationAwarenessWatcherEventId{};
        size_t _microphoneWatcherEventId{};
        size_t _clientStateChangedEventId{};
        size_t _settingsUpdateEventId{};

        static int64_t GetSteadyMilliseconds();
        void StartMonitor();
        void StopMonitor();
        void MonitorLoop();
        bool StartCapture(const std::optional<std::string>& application);
        void StopCapture();
        void DecodeLoop(std::unique_ptr<class AacEldDecoder> decoder, std::unique_ptr<VirtualMicrophoneOutput> output);
        void ResetA2dp();
        void UpdateCaptureState(bool active, const std::optional<std::string>& application = std::nullopt);
        void NotifyLevelChanged();
        bool GetDeviceSetting(const std::string& settingName, bool defaultValue);
        bool GetGlobalSetting(const std::string& settingName, bool defaultValue);
        void OnSettingUpdated(const UpdatedSettingNotification& notification);
        void SetConversationAwareness(bool enabled);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;
        void Reset() override;

    public:
        explicit AapHighResolutionMicrophoneCapability(AapDevice& device);
        ~AapHighResolutionMicrophoneCapability() override;
        static bool IsSupported(unsigned short model);
        void SetFromJson(const nlohmann::json& json) override;
    };
}
