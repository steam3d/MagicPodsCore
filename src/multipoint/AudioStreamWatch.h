// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// is OUR audio on the buds right now

#pragma once

#include <atomic>
#include <string>
#include <sys/types.h>
#include <thread>

namespace MagicPodsCore
{
    // Watches whether this host's audio stream to the buds is actually running, which is the
    // only local signal that says "our audio just went onto the wire". The PC side needed the
    // same event to time its focus claim (20.9/20.11): while the peer
    // holds the focus the buds themselves send AVRCP KEY_PAUSECD at an unauthorised stream
    // 0.8-1.2 s after it starts, so a claim has to be on the wire before that - and the
    // PipeWire sink going RUNNING is the moment the stream starts.
    //
    // Implementation: a `pactl subscribe` child process; every event naming our buds' bluez
    // sink triggers one cheap `pactl list sinks` state query. Index numbers move when the
    // device reconnects, so everything matches by sink NAME (bench lesson, 20.11).
    class AudioStreamWatch
    {
    public:
        // budsAddress: the buds' BD_ADDR as BlueZ reports it ("64:5D:F4:E1:79:74"); the sink
        // is bluez_output.64_5D_F4_E1_79_74.<profile>.
        explicit AudioStreamWatch(std::string budsAddress);
        ~AudioStreamWatch();

        void Start();
        void Stop();

        // True while the buds' sink reports State: RUNNING (the stream is carrying data).
        bool IsOutputActive() const { return _outputActive; }

    private:
        void RunReader();
        // One `pactl list sinks` pass, name-matched, updates _outputActive.
        void QuerySinkState();

        std::string _sinkNamePrefix{}; // "bluez_output.64_5D_F4_E1_79_74."
        int _pipeFd[2] = {-1, -1};
        pid_t _childPid = -1;
        std::thread _reader{};
        std::atomic<bool> _running{false};
        std::atomic<bool> _outputActive{false};
    };
}
