// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <pulse/pulseaudio.h>
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <atomic>
#include <thread>
#include "Event.h"

namespace MagicPodsCore{

    struct CardInfo {
        uint32_t index{PA_INVALID_INDEX};
        std::string name;
        std::string activeProfile;
        std::vector<std::pair<std::string,std::string>> profiles;

    bool operator==(const CardInfo& other) const {
        return index == other.index &&
               name == other.name &&
               activeProfile == other.activeProfile &&
               profiles == other.profiles;
    }
    };

    struct SinkInfo {
        uint32_t index{PA_INVALID_INDEX};
        uint32_t card{PA_INVALID_INDEX};
        std::string name;
        std::string cardName;
        std::string activePort;
        pa_cvolume volume{};

        bool IsOnCard(const CardInfo& cardInfo) const {
            return card == cardInfo.index || cardName == cardInfo.name;
        }
    };
    
    class PulseAudioClient{
        public:
            PulseAudioClient();
            ~PulseAudioClient();
            bool SetCardProfile(const std::string& name, const std::string& profile);
            std::optional<CardInfo> GetCardInfoByName(const std::string& name);
            std::optional<CardInfo> GetCardInfoByIndex(uint32_t index);
            std::optional<uint32_t> LoadModule(const std::string& name, const std::string& arguments);
            bool UnloadModule(uint32_t index);
            bool SetDefaultSource(const std::string& sourceName);
            std::optional<std::string> GetDefaultSinkName();
            std::optional<SinkInfo> GetSinkInfoByName(const std::string& name);
            std::vector<SinkInfo> GetSinks();
            bool SetDefaultSink(const std::string& sinkName);
            bool MoveSinkInputs(uint32_t sourceSinkIndex, uint32_t destinationSinkIndex);
            bool SwitchCardToSpeakers(const CardInfo& card);
            bool SwitchToCard(const CardInfo& card);
            bool FadeSinkVolume(uint32_t sinkIndex, const pa_cvolume& from, const pa_cvolume& to);
            std::vector<uint32_t> GetModuleIndexesByArgument(const std::string& value);
            std::optional<std::string> GetSourceConsumer(const std::string& sourceName);
            std::string GetNameFromMac(const std::string& mac);
            Event<CardInfo>& GatAudioCardPropertyChangedEvent() {
                return _onAudioCardPropertyChangedEvent;
            }
        private:
            Event<CardInfo> _onAudioCardPropertyChangedEvent{};
            std::atomic<bool> ready{false};
            std::atomic<bool> stop{false};
            std::thread th;
            pa_mainloop* ml {nullptr};
            pa_context* ctx {nullptr};
            void MainLoop();
            void StopLoop();
            bool WaitReady(std::chrono::milliseconds timeout = std::chrono::seconds(3));
            bool SwitchOutput(const SinkInfo& source, const SinkInfo& destination);
            void Free();

    };
}
