// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <sdbus-c++/sdbus-c++.h>

namespace MagicPodsCore
{
    class MediaController
    {
    private:
        std::unique_ptr<sdbus::IConnection> _sessionBus{};
        std::unique_ptr<sdbus::IProxy> _dbusProxy{};
        std::vector<std::string> _pausedPlayers{};
        std::mutex _mutex{};

        bool Connect();
        std::vector<std::string> GetPlayerNames();
        bool WasPaused(const std::string& playerName) const;

    public:
        void Pause();
        void Play();
    };
}
