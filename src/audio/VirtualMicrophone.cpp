// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#include "VirtualMicrophone.h"
#include "Logger.h"
#include "StringUtils.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

namespace MagicPodsCore
{
    VirtualMicrophoneOutput::VirtualMicrophoneOutput(const std::string& path, bool useAutomaticGainControl)
    {
        _fileDescriptor = open(path.c_str(), O_RDWR | O_CLOEXEC);
        if (_fileDescriptor < 0)
        {
            Logger::Error("Failed to open virtual microphone FIFO %s: %s", path.c_str(), strerror(errno));
            return;
        }

        if (useAutomaticGainControl)
            _automaticGainControl.emplace();
        else
            Logger::Info("Virtual microphone AGC is disabled");
    }

    VirtualMicrophoneOutput::~VirtualMicrophoneOutput()
    {
        if (_fileDescriptor >= 0)
            close(_fileDescriptor);
    }

    bool VirtualMicrophoneOutput::IsOpen() const
    {
        return _fileDescriptor >= 0;
    }

    std::optional<float> VirtualMicrophoneOutput::Write(std::span<const int16_t> samples)
    {
        if (!IsOpen())
            return std::nullopt;

        std::vector<int16_t> processed{};
        if (_automaticGainControl.has_value())
        {
            processed.assign(samples.begin(), samples.end());
            _automaticGainControl->Process(processed);
            samples = std::span<const int16_t>{processed};
        }

        float peak = 0.0f;
        for (const auto sample : samples)
            peak = std::max(peak, std::abs(static_cast<float>(sample) / 32768.0f));

        const auto* bytes = reinterpret_cast<const unsigned char*>(samples.data());
        size_t bytesLeft = samples.size_bytes();
        while (bytesLeft > 0)
        {
            const ssize_t written = write(_fileDescriptor, bytes, bytesLeft);
            if (written < 0)
            {
                if (errno == EINTR)
                    continue;
                Logger::Error("Virtual microphone FIFO write failed: %s", strerror(errno));
                return std::nullopt;
            }
            bytes += written;
            bytesLeft -= written;
        }

        return peak;
    }

    std::string VirtualMicrophone::GetFifoPath()
    {
        if (const char* runtimeDirectory = std::getenv("XDG_RUNTIME_DIR"))
            return std::string{runtimeDirectory} + "/magicpods-virtual-mic.fifo";

        return StringUtils::Format("/tmp/magicpods-virtual-mic-%u.fifo", static_cast<unsigned int>(geteuid()));
    }

    VirtualMicrophone::VirtualMicrophone(std::shared_ptr<PulseAudioClient> audioClient, uint32_t sampleRate, uint8_t channels)
        : _audioClient{std::move(audioClient)}, _fifoPath{GetFifoPath()}
    {
        for (const auto index : _audioClient->GetModuleIndexesByArgument(SourceName))
        {
            Logger::Warn("Unloading stale virtual microphone module %u", index);
            _audioClient->UnloadModule(index);
        }

        unlink(_fifoPath.c_str());

        const std::string channelMap = channels == 1 ? "mono" : "front-left,front-right";
        const std::string arguments = StringUtils::Format(
            "source_name=%s file=%s format=s16le rate=%u channels=%u channel_map=%s "
            "source_properties=\"device.description='%s' node.description='%s' node.driver=false priority.driver=0\"",
            SourceName,
            _fifoPath.c_str(),
            sampleRate,
            channels,
            channelMap.c_str(),
            Description,
            Description);

        _moduleIndex = _audioClient->LoadModule("module-pipe-source", arguments);
        if (!_moduleIndex.has_value())
        {
            Logger::Error("Failed to create %s", Description);
            unlink(_fifoPath.c_str());
            return;
        }

        if (!_audioClient->SetDefaultSource(SourceName))
            Logger::Warn("Failed to set %s as the default source", Description);

        Logger::Info("Virtual microphone is ready: %s", Description);
    }

    VirtualMicrophone::~VirtualMicrophone()
    {
        if (_moduleIndex.has_value())
            _audioClient->UnloadModule(_moduleIndex.value());
        unlink(_fifoPath.c_str());
    }

    bool VirtualMicrophone::IsOpen() const
    {
        return _moduleIndex.has_value();
    }

    std::unique_ptr<VirtualMicrophoneOutput> VirtualMicrophone::OpenOutput(bool useAutomaticGainControl) const
    {
        auto output = std::make_unique<VirtualMicrophoneOutput>(_fifoPath, useAutomaticGainControl);
        if (!output->IsOpen())
            return nullptr;
        return output;
    }

    std::optional<std::string> VirtualMicrophone::GetConsumer() const
    {
        if (!IsOpen())
            return std::nullopt;
        return _audioClient->GetSourceConsumer(SourceName);
    }
}
