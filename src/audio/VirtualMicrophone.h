// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#pragma once

#include "AutomaticGainControl.h"
#include "pulseaudio/PulseAudioClient.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>

namespace MagicPodsCore
{
    class VirtualMicrophoneOutput
    {
    private:
        int _fileDescriptor{-1};
        std::optional<AutomaticGainControl> _automaticGainControl{};

    public:
        VirtualMicrophoneOutput(const std::string& path, bool useAutomaticGainControl);
        ~VirtualMicrophoneOutput();

        VirtualMicrophoneOutput(const VirtualMicrophoneOutput&) = delete;
        VirtualMicrophoneOutput& operator=(const VirtualMicrophoneOutput&) = delete;

        bool IsOpen() const;
        std::optional<float> Write(std::span<const int16_t> samples);
    };

    class VirtualMicrophone
    {
    private:
        std::shared_ptr<PulseAudioClient> _audioClient{};
        std::optional<uint32_t> _moduleIndex{};
        std::string _fifoPath{};

        static std::string GetFifoPath();

    public:
        static constexpr auto SourceName = "MagicPodsVirtualMic";
        static constexpr auto Description = "MagicPods Virtual Mic";

        VirtualMicrophone(std::shared_ptr<PulseAudioClient> audioClient, uint32_t sampleRate, uint8_t channels);
        ~VirtualMicrophone();

        VirtualMicrophone(const VirtualMicrophone&) = delete;
        VirtualMicrophone& operator=(const VirtualMicrophone&) = delete;

        bool IsOpen() const;
        std::unique_ptr<VirtualMicrophoneOutput> OpenOutput(bool useAutomaticGainControl) const;
        std::optional<std::string> GetConsumer() const;
    };
}
