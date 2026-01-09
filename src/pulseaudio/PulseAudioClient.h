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
        std::string name;
        std::string activeProfile;
        std::vector<std::pair<std::string,std::string>> profiles;

    bool operator==(const CardInfo& other) const {
        return name == other.name &&
               activeProfile == other.activeProfile &&
               profiles == other.profiles;
    }
    };
    
    class PulseAudioClient{
        public:
            PulseAudioClient();
            ~PulseAudioClient();
            bool SetCardProfile(const std::string& name, const std::string& profile);
            std::optional<CardInfo> GetCardInfoByName(const std::string& name);
            std::optional<CardInfo> GetCardInfoByIndex(uint32_t index);
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
            void Free();

    };
}
