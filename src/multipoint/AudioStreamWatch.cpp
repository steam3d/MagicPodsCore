// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// is OUR audio on the buds right now

#include "AudioStreamWatch.h"

#include "Logger.h"

#include <algorithm>
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <vector>
#include <unistd.h>

namespace MagicPodsCore
{
    namespace
    {
        std::string ToLower(std::string value)
        {
            std::transform(value.begin(), value.end(), value.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return value;
        }
    }

    AudioStreamWatch::AudioStreamWatch(std::string budsAddress)
    {
        // "64:5D:F4:E1:79:74" -> "bluez_output.64_5D_F4_E1_79_74." (matches every profile the
        // device can expose; the state line we care about is the a2dp-sink one).
        std::string tail = std::move(budsAddress);
        std::replace(tail.begin(), tail.end(), ':', '_');
        _sinkNamePrefix = "bluez_output." + tail + ".";
    }

    AudioStreamWatch::~AudioStreamWatch()
    {
        Stop();
    }

    void AudioStreamWatch::Start()
    {
        if (_running)
        {
            return;
        }
        // A previous pactl instance may have died (RunReader cleared _running): reap it
        // before re-arming, or the dead reader thread and its child linger forever.
        if (_reader.joinable())
        {
            _reader.join();
        }
        if (_childPid > 0)
        {
            waitpid(_childPid, nullptr, 0);
            _childPid = -1;
        }
        if (_pipeFd[0] >= 0)
        {
            close(_pipeFd[0]);
            _pipeFd[0] = -1;
        }
        if (pipe(_pipeFd) != 0)
        {
            Logger::Warn("AudioStreamWatch: pipe() failed, audio takeover stays dark");
            return;
        }

        const pid_t pid = fork();
        if (pid < 0)
        {
            Logger::Warn("AudioStreamWatch: fork() failed, audio takeover stays dark");
            close(_pipeFd[0]);
            close(_pipeFd[1]);
            return;
        }
        if (pid == 0)
        {
            // Child: pactl's stdout/stderr into the pipe, then exec. _exit, not exit: the
            // atexit handlers of a forked audio-stack client have nothing to do here.
            dup2(_pipeFd[1], STDOUT_FILENO);
            dup2(_pipeFd[1], STDERR_FILENO);
            close(_pipeFd[0]);
            close(_pipeFd[1]);
            execlp("pactl", "pactl", "subscribe", static_cast<char*>(nullptr));
            _exit(127);
        }

        _childPid = pid;
        close(_pipeFd[1]);
        _pipeFd[1] = -1;
        _running = true;
        _reader = std::thread{[this] { RunReader(); }};
        // Seed the current state so the first edge we see is a real one.
        QuerySinkState();
    }

    void AudioStreamWatch::Stop()
    {
        if (!_running)
        {
            return;
        }
        _running = false;
        if (_childPid > 0)
        {
            kill(_childPid, SIGTERM);
        }
        if (_reader.joinable())
        {
            _reader.join();
        }
        if (_childPid > 0)
        {
            waitpid(_childPid, nullptr, 0);
            _childPid = -1;
        }
        _outputActive = false;
    }

    void AudioStreamWatch::RunReader()
    {
        std::string pending{};
        std::vector<char> buffer(1024);
        while (_running)
        {
            const ssize_t got = read(_pipeFd[0], buffer.data(), buffer.size());
            if (got <= 0)
            {
                // EOF (pactl died or was terminated) or error: nothing more to learn.
                break;
            }
            pending.append(buffer.data(), static_cast<size_t>(got));
            size_t newline = 0;
            while ((newline = pending.find('\n')) != std::string::npos)
            {
                std::string line = pending.substr(0, newline);
                pending.erase(0, newline + 1);
                const std::string lower = ToLower(std::move(line));
                // Events naming any sink of OUR buds (change/remove) need a re-read; sink
                // inputs and unrelated sinks are ignored. Matching is by name, never index.
                if (lower.find("on sink ") != std::string::npos &&
                    lower.find(ToLower(_sinkNamePrefix)) != std::string::npos)
                {
                    QuerySinkState();
                }
            }
        }
        if (_pipeFd[0] >= 0)
        {
            close(_pipeFd[0]);
            _pipeFd[0] = -1;
        }
    }

    void AudioStreamWatch::QuerySinkState()
    {
        FILE* pipe = popen("pactl list sinks 2>/dev/null", "r");
        if (!pipe)
        {
            return;
        }
        // pactl prints blocks: "Sink #NNN", then "State: ...", "Name: ..." among others.
        // Index numbers churn on reconnects (bench 20.11), so match the name only.
        const std::string prefix = ToLower(_sinkNamePrefix);
        bool active = false;
        std::string name{};
        std::string state{};
        const auto finishBlock = [&]
        {
            if (!name.empty() && state == "RUNNING" && name.find(prefix) != std::string::npos)
            {
                active = true;
            }
            name.clear();
            state.clear();
        };
        char line[512] = {0};
        while (fgets(line, sizeof(line), pipe))
        {
            const std::string text{line};
            if (text.rfind("Sink #", 0) == 0)
            {
                finishBlock();
                continue;
            }
            if (text.rfind("Name: ", 0) == 0)
            {
                name = ToLower(text.substr(6));
            }
            else if (text.rfind("State: ", 0) == 0)
            {
                // "State: RUNNING" (or SUSPENDED / IDLE); a trailing profile suffix appears
                // on some pactl versions, only an exact RUNNING counts.
                std::string value = text.substr(7);
                const size_t space = value.find_first_of(" \n");
                value.resize(space == std::string::npos ? value.size() : space);
                state = std::move(value);
            }
        }
        finishBlock();
        pclose(pipe);
        _outputActive = active;
    }
}
