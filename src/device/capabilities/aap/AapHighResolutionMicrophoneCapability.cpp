// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#include "AapHighResolutionMicrophoneCapability.h"

#include "Logger.h"
#include "audio/AacEldDecoder.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/setters/AapSetMicrophoneOff.h"
#include "sdk/aap/setters/AapSetMicrophoneOn.h"

namespace MagicPodsCore
{
    namespace
    {
        constexpr auto GlobalSettingsContainer = "magicpods";
        constexpr auto EnabledSetting = "aap_mic";
        constexpr auto AgcSetting = "aap_mic_agc";
        constexpr auto ResetA2dpSetting = "aap_mic_a2dp_reset";
        constexpr auto KeepActiveSetting = "aap_mic_keep_alive";
    }

    int64_t AapHighResolutionMicrophoneCapability::GetSteadyMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    bool AapHighResolutionMicrophoneCapability::GetDeviceSetting(const std::string& settingName, bool defaultValue)
    {
        const auto value = device.LoadSettingBool(settingName);
        return value.has_value() ? value.value() : defaultValue;
    }

    bool AapHighResolutionMicrophoneCapability::GetGlobalSetting(
        const std::string& settingName,
        bool defaultValue)
    {
        const auto value = _settingsService->GetSetting(GlobalSettingsContainer, settingName);
        if (value.is_boolean())
            return value.as_boolean()->get();

        Logger::Error("Failed to read global boolean setting %s", settingName.c_str());
        return defaultValue;
    }

    void AapHighResolutionMicrophoneCapability::OnSettingUpdated(
        const UpdatedSettingNotification& notification)
    {
        if (notification.GetContainerName() != GlobalSettingsContainer)
            return;

        const auto settingName = notification.GetSettingName();
        if (settingName != AgcSetting &&
            settingName != ResetA2dpSetting &&
            settingName != KeepActiveSetting)
        {
            return;
        }

        const auto value = notification.GetValue();
        if (!value.is_boolean())
        {
            Logger::Error("Global microphone setting %s must be boolean",
                std::string{settingName}.c_str());
            return;
        }

        const bool enabled = value.as_boolean()->get();
        {
            std::lock_guard lock{_stateMutex};
            if (settingName == AgcSetting)
                _useAutomaticGainControl = enabled;
            else if (settingName == ResetA2dpSetting)
                _resetA2dp = enabled;
            else if (settingName == KeepActiveSetting)
                _keepMicrophoneActive = enabled;
        }

        if (settingName == KeepActiveSetting)
            _monitorCondition.notify_all();
    }

    AapHighResolutionMicrophoneCapability::AapHighResolutionMicrophoneCapability(AapDevice& device)
        : AapCapability("highResolutionMicrophone", false, device)
    {
        _settingsService = device.GetSettingsService();
        _enabled = GetDeviceSetting(EnabledSetting, false);
        _useAutomaticGainControl = GetGlobalSetting(AgcSetting, true);
        _resetA2dp = GetGlobalSetting(ResetA2dpSetting, true);
        _keepMicrophoneActive = GetGlobalSetting(KeepActiveSetting, true);

        _settingsUpdateEventId = _settingsService->GetOnSettingUpdateEvent().Subscribe(
            [this](size_t, const UpdatedSettingNotification& notification)
            {
                OnSettingUpdated(notification);
            });

        _microphoneWatcherEventId = _microphoneWatcher.GetEvent().Subscribe(
            [this](size_t, const AapMicrophoneWatcher::AudioFrames& frames)
            {
                if (!_microphoneStreamActive.load())
                    return;

                _lastAudioPacketTime = GetSteadyMilliseconds();
                if (!_captureActive.load())
                    return;

                {
                    std::lock_guard lock{_audioQueueMutex};
                    if (_audioQueue.size() >= AudioQueueCapacity)
                        _audioQueue.pop_front();
                    _audioQueue.push_back(frames);
                }
                _audioQueueCondition.notify_one();
            });

        _clientStateChangedEventId = device.GetClientStateChangedEvent().Subscribe(
            [this](size_t, ClientState state)
            {
                if (state == ClientState::Connected)
                {
                    const bool wasAvailable = isAvailable;
                    isAvailable = true;
                    StartMonitor();
                    if (!wasAvailable)
                        _onChanged.FireEvent(*this);
                }
                else if (state == ClientState::Reconnecting)
                {
                    Reset();
                    _onChanged.FireEvent(*this);
                }
            });
    }

    AapHighResolutionMicrophoneCapability::~AapHighResolutionMicrophoneCapability()
    {
        _settingsService->GetOnSettingUpdateEvent().Unsubscribe(_settingsUpdateEventId);
        StopMonitor();
        _microphoneWatcher.GetEvent().Unsubscribe(_microphoneWatcherEventId);
    }

    bool AapHighResolutionMicrophoneCapability::IsSupported(unsigned short model)
    {
        // Only AirPods with the H2 chip are supported; other AirPods and Beats are not supported.
        switch (static_cast<AapModelIds>(model))
        {
        case AapModelIds::airpods4:
        case AapModelIds::airpods4anc:
        case AapModelIds::airpods5wcc:
        case AapModelIds::airpods5:
        case AapModelIds::airpodsprousbc:
        case AapModelIds::airpodspro2:
        case AapModelIds::airpodspro3:
        case AapModelIds::airpodsmax2:
            return true;
        default:
            return false;
        }
    }

    nlohmann::json AapHighResolutionMicrophoneCapability::CreateJsonBody()
    {
        std::lock_guard lock{_stateMutex};
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = _enabled;
        // bodyJson["active"] = _active.load();
        // bodyJson["level"] = _level.load();
        // if (_application.empty())
        //     bodyJson["application"] = nullptr;
        // else
        //     bodyJson["application"] = _application;
        // bodyJson["device"] = VirtualMicrophone::Description;
        return bodyJson;
    }

    void AapHighResolutionMicrophoneCapability::OnReceivedData(const std::vector<unsigned char>& data)
    {
        _microphoneWatcher.ProcessResponse(data);
    }

    void AapHighResolutionMicrophoneCapability::Reset()
    {
        StopMonitor();
        UpdateCaptureState(false);
        Capability::Reset();
    }

    void AapHighResolutionMicrophoneCapability::StartMonitor()
    {
        bool enabled;
        {
            std::lock_guard lock{_stateMutex};
            enabled = _enabled;
        }
        if (!enabled || device.GetClientState() != ClientState::Connected)
            return;

        std::lock_guard lifecycleLock{_monitorLifecycleMutex};
        if (_monitorRunning.load())
        {
            _monitorCondition.notify_all();
            return;
        }

        if (_monitorThread.joinable())
            _monitorThread.join();

        _stopMonitor = false;
        _monitorRunning = true;
        _monitorThread = std::thread(&AapHighResolutionMicrophoneCapability::MonitorLoop, this);
    }

    void AapHighResolutionMicrophoneCapability::StopMonitor()
    {
        {
            std::lock_guard lifecycleLock{_monitorLifecycleMutex};
            _stopMonitor = true;
            _monitorCondition.notify_all();
            _audioQueueCondition.notify_all();
        }

        if (_monitorThread.joinable() && _monitorThread.get_id() != std::this_thread::get_id())
            _monitorThread.join();
        {
            std::lock_guard lifecycleLock{_monitorLifecycleMutex};
            _monitorRunning = false;
        }
    }

    void AapHighResolutionMicrophoneCapability::MonitorLoop()
    {
        _virtualMicrophone = std::make_unique<VirtualMicrophone>(
            device.GetAudioClient(), AacEldDecoder::SampleRate, AacEldDecoder::Channels);
        if (!_virtualMicrophone->IsOpen())
        {
            _virtualMicrophone.reset();
            _monitorRunning = false;
            return;
        }

        Logger::Info("High-resolution microphone monitor started for %s", device.GetName().c_str());
        std::optional<std::chrono::steady_clock::time_point> pendingA2dpReset{};

        while (!_stopMonitor.load())
        {
            const auto application = _virtualMicrophone->GetConsumer();
            const bool recording = application.has_value();
            bool enabled;
            bool keepMicrophoneActive;
            {
                std::lock_guard lock{_stateMutex};
                enabled = _enabled;
                keepMicrophoneActive = _keepMicrophoneActive;
            }
            const bool connected = device.GetClientState() == ClientState::Connected;
            const bool shouldCapture = enabled && connected && recording;

            if (shouldCapture && !_captureActive.load())
            {
                Logger::Info("Virtual microphone consumer detected: %s", application->c_str());
                StartCapture(application);
            }
            else if (_captureActive.load() && !shouldCapture)
            {
                Logger::Info("Stopping high-resolution microphone capture");
                StopCapture();
            }
            else if (_captureActive.load())
            {
                UpdateCaptureState(true, application);
                if (_outputBroken.load())
                {
                    Logger::Warn("High-resolution microphone output failed, restarting capture");
                    StopCapture();
                    StartCapture(application);
                }
            }

            const bool shouldStream = enabled && connected &&
                (keepMicrophoneActive || _captureActive.load());
            if (shouldStream && !_microphoneStreamActive.load())
            {
                StartMicrophoneStream();
                pendingA2dpReset = std::chrono::steady_clock::now() + A2dpResetDelay;
            }
            else if (_microphoneStreamActive.load() && !shouldStream)
            {
                StopMicrophoneStream();
                pendingA2dpReset.reset();
            }

            const bool stalled = _microphoneStreamActive.load() &&
                GetSteadyMilliseconds() - _lastAudioPacketTime.load() > StallTimeout.count();
            if (stalled)
            {
                Logger::Warn("High-resolution microphone stream stalled, restarting stream");
                if (_captureActive.load())
                    StopCapture();
                StopMicrophoneStream();

                if (shouldCapture)
                    StartCapture(application);
                const bool shouldRestartStream = enabled && connected &&
                    (keepMicrophoneActive || _captureActive.load());
                if (shouldRestartStream)
                {
                    StartMicrophoneStream();
                    pendingA2dpReset = std::chrono::steady_clock::now() + A2dpResetDelay;
                }
                else
                {
                    pendingA2dpReset.reset();
                }
            }

            if (pendingA2dpReset.has_value() &&
                std::chrono::steady_clock::now() >= pendingA2dpReset.value())
            {
                ResetA2dp();
                pendingA2dpReset.reset();
            }

            if (!enabled && !recording)
            {
                // Serialize the final decision with StartMonitor(). If the
                // setting was enabled again while this iteration was running,
                // keep the existing virtual source instead of losing the wake.
                std::lock_guard lifecycleLock{_monitorLifecycleMutex};
                std::lock_guard stateLock{_stateMutex};
                if (!_enabled)
                {
                    _monitorRunning = false;
                    break;
                }
            }

            std::unique_lock lock{_monitorMutex};
            _monitorCondition.wait_for(lock, MonitorInterval, [this]() { return _stopMonitor.load(); });
        }

        if (_captureActive.load())
            StopCapture();
        if (_microphoneStreamActive.load())
            StopMicrophoneStream();
        _virtualMicrophone.reset();
        _monitorRunning = false;
        Logger::Info("High-resolution microphone monitor stopped for %s", device.GetName().c_str());
    }

    void AapHighResolutionMicrophoneCapability::StartMicrophoneStream()
    {
        if (_microphoneStreamActive.exchange(true))
            return;

        _lastAudioPacketTime = GetSteadyMilliseconds();
        SendData(AapSetMicrophoneOn{});
        Logger::Info("AirPods high-resolution microphone stream started");
    }

    void AapHighResolutionMicrophoneCapability::StopMicrophoneStream()
    {
        if (!_microphoneStreamActive.exchange(false))
            return;

        if (device.GetClientState() == ClientState::Connected)
            SendData(AapSetMicrophoneOff{});

        if (device.GetClientState() == ClientState::Connected)
            ResetA2dp();

        Logger::Info("AirPods high-resolution microphone stream stopped");
    }

    bool AapHighResolutionMicrophoneCapability::StartCapture(const std::optional<std::string>& application)
    {
        auto decoder = std::make_unique<AacEldDecoder>();
        if (!decoder->IsOpen())
            return false;

        bool useAutomaticGainControl;
        {
            std::lock_guard lock{_stateMutex};
            useAutomaticGainControl = _useAutomaticGainControl;
        }

        auto output = _virtualMicrophone->OpenOutput(useAutomaticGainControl);
        if (!output)
            return false;

        {
            std::lock_guard lock{_audioQueueMutex};
            _audioQueue.clear();
        }
        _outputBroken = false;
        _captureActive = true;
        _decodeThread = std::thread(
            &AapHighResolutionMicrophoneCapability::DecodeLoop,
            this,
            std::move(decoder),
            std::move(output));

        UpdateCaptureState(true, application);
        return true;
    }

    void AapHighResolutionMicrophoneCapability::StopCapture()
    {
        if (!_captureActive.exchange(false))
            return;

        {
            std::lock_guard lock{_audioQueueMutex};
            _audioQueue.clear();
        }
        _audioQueueCondition.notify_all();
        if (_decodeThread.joinable())
            _decodeThread.join();

        UpdateCaptureState(false);
        Logger::Info("High-resolution microphone capture stopped");
    }

    void AapHighResolutionMicrophoneCapability::DecodeLoop(
        std::unique_ptr<AacEldDecoder> decoder,
        std::unique_ptr<VirtualMicrophoneOutput> output)
    {
        uint64_t decodedFrames = 0;
        uint64_t errors = 0;
        float levelEnvelope = 0.0f;
        std::vector<int16_t> pcm{};
        pcm.reserve(4096);

        while (_captureActive.load())
        {
            AapMicrophoneWatcher::AudioFrames frames{};
            {
                std::unique_lock lock{_audioQueueMutex};
                _audioQueueCondition.wait(lock, [this]()
                {
                    return !_captureActive.load() || !_audioQueue.empty();
                });

                if (!_captureActive.load())
                    break;

                frames = std::move(_audioQueue.front());
                _audioQueue.pop_front();
            }

            pcm.clear();
            for (const auto& accessUnit : frames)
            {
                if (decoder->Decode(std::span<const unsigned char>{accessUnit}, pcm))
                    ++decodedFrames;
                else
                {
                    ++errors;
                    pcm.insert(pcm.end(), AacEldDecoder::FrameSamples * AacEldDecoder::Channels, 0);
                }
            }

            if (pcm.empty())
                continue;

            const auto peak = output->Write(pcm);
            if (!peak.has_value())
            {
                _outputBroken = true;
                _monitorCondition.notify_all();
                break;
            }

            levelEnvelope = peak.value() >= levelEnvelope ? peak.value() : levelEnvelope * LevelRelease;
            _level = levelEnvelope;
            // NotifyLevelChanged(); // Enable when level is exposed in the capability JSON.

            if (decodedFrames > 0 && decodedFrames % 400 == 0)
            {
                const double seconds = static_cast<double>(decodedFrames * AacEldDecoder::FrameSamples) /
                                       AacEldDecoder::SampleRate;
                Logger::Info("High-resolution microphone: %llu frames (%.0fs), %llu errors, level %.2f",
                    static_cast<unsigned long long>(decodedFrames),
                    seconds,
                    static_cast<unsigned long long>(errors),
                    levelEnvelope);
            }
        }

        _level = 0.0f;
        Logger::Info("High-resolution microphone decoder stopped: %llu frames, %llu errors",
            static_cast<unsigned long long>(decodedFrames),
            static_cast<unsigned long long>(errors));
    }

    void AapHighResolutionMicrophoneCapability::ResetA2dp()
    {
        bool resetA2dp;
        {
            std::lock_guard lock{_stateMutex};
            resetA2dp = _resetA2dp;
        }
        if (!resetA2dp)
            return;

        auto audioClient = device.GetAudioClient();
        const std::string cardName = audioClient->GetNameFromMac(device.GetAddress());
        const auto cardInfo = audioClient->GetCardInfoByName(cardName);
        if (!cardInfo.has_value() || cardInfo->activeProfile.empty() || cardInfo->activeProfile == "off")
        {
            Logger::Warn("No active Bluetooth profile on %s, skipping A2DP reset", cardName.c_str());
            return;
        }

        Logger::Info("Resetting A2DP transport: %s off -> %s", cardName.c_str(), cardInfo->activeProfile.c_str());
        device.PauseMedia();
        audioClient->SetCardProfile(cardName, "off");
        audioClient->SetCardProfile(cardName, cardInfo->activeProfile);
        device.PlayMedia();
    }

    void AapHighResolutionMicrophoneCapability::UpdateCaptureState(
        bool active,
        const std::optional<std::string>& application)
    {
        // const bool changed = _active.exchange(active) != active; // Enable when active is exposed in JSON.
        _active.store(active);
        {
            std::lock_guard lock{_stateMutex};
            const std::string newApplication = active && application.has_value() ? application.value() : std::string{};
            // changed = changed || _application != newApplication; // Enable when application is exposed in JSON.
            _application = newApplication;
        }
        if (!active)
            _level = 0.0f;
        // if (changed)
        //     _onChanged.FireEvent(*this);
    }

    void AapHighResolutionMicrophoneCapability::NotifyLevelChanged()
    {
        const int64_t now = GetSteadyMilliseconds();
        int64_t previous = _lastLevelNotificationTime.load();
        if (now - previous < 100 || !_lastLevelNotificationTime.compare_exchange_strong(previous, now))
            return;
        _onChanged.FireEvent(*this);
    }

    void AapHighResolutionMicrophoneCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name) || !json.at(name).is_object())
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected"))
            return;
        if (!capability.at("selected").is_boolean())
        {
            Logger::Error("Error: %s::SetFromJson got a non-boolean selected", name.c_str());
            return;
        }

        bool enabledBefore;
        bool enabledAfter;
        {
            std::lock_guard lock{_stateMutex};
            enabledBefore = _enabled;
            _enabled = capability.at("selected").get<bool>();
            enabledAfter = _enabled;
        }

        if (enabledBefore == enabledAfter)
            return;

        device.SaveSettingBool(EnabledSetting, enabledAfter);

        if (enabledAfter)
            StartMonitor();
        else
            _monitorCondition.notify_all();

        _onChanged.FireEvent(*this);
    }
}
