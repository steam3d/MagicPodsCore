// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "MediaController.h"
#include "Logger.h"

#include <algorithm>

namespace MagicPodsCore
{
    namespace
    {
        constexpr auto DBUS_SERVICE = "org.freedesktop.DBus";
        constexpr auto DBUS_PATH = "/org/freedesktop/DBus";
        constexpr auto DBUS_INTERFACE = "org.freedesktop.DBus";
        constexpr auto MPRIS_PATH = "/org/mpris/MediaPlayer2";
        constexpr auto MPRIS_PLAYER_INTERFACE = "org.mpris.MediaPlayer2.Player";
        constexpr auto MPRIS_SERVICE_PREFIX = "org.mpris.MediaPlayer2.";
    }

    bool MediaController::Connect()
    {
        if (_sessionBus && _dbusProxy)
            return true;

        try
        {
            // Media players are available on the desktop user's session bus.
            _sessionBus = sdbus::createSessionBusConnection();
            _dbusProxy = sdbus::createProxy(*_sessionBus, DBUS_SERVICE, DBUS_PATH);
            return true;
        }
        catch (const sdbus::Error& error)
        {
            _dbusProxy.reset();
            _sessionBus.reset();
            Logger::Error("Failed to connect to the session D-Bus: %s", error.getMessage().c_str());
            return false;
        }
    }

    std::vector<std::string> MediaController::GetPlayerNames()
    {
        std::vector<std::string> serviceNames{};
        std::vector<std::string> playerNames{};

        _dbusProxy->callMethod("ListNames")
            .onInterface(DBUS_INTERFACE)
            .storeResultsTo(serviceNames);

        for (const auto& serviceName : serviceNames)
        {
            if (serviceName.starts_with(MPRIS_SERVICE_PREFIX))
                playerNames.push_back(serviceName);
        }

        return playerNames;
    }

    bool MediaController::WasPaused(const std::string& playerName) const
    {
        return std::find(_pausedPlayers.begin(), _pausedPlayers.end(), playerName) != _pausedPlayers.end();
    }

    void MediaController::Pause()
    {
        // Only one thread may use the D-Bus connection and the player list at a time.
        std::lock_guard lock{_mutex};

        if (!Connect())
            return;

        try
        {
            for (const auto& playerName : GetPlayerNames())
            {
                try
                {
                    auto playerProxy = sdbus::createProxy(*_sessionBus, playerName, MPRIS_PATH);
                    std::string playbackStatus = playerProxy->getProperty("PlaybackStatus")
                        .onInterface(MPRIS_PLAYER_INTERFACE);

                    if (playbackStatus != "Playing")
                        continue;

                    playerProxy->callMethod("Pause").onInterface(MPRIS_PLAYER_INTERFACE);

                    // Play() must resume only players paused by this controller.
                    if (!WasPaused(playerName))
                        _pausedPlayers.push_back(playerName);

                    Logger::Debug("Media player paused: %s", playerName.c_str());
                }
                catch (const sdbus::Error& error)
                {
                    Logger::Warn("Failed to pause media player %s: %s", playerName.c_str(), error.getMessage().c_str());
                }
            }
        }
        catch (const sdbus::Error& error)
        {
            Logger::Error("Failed to get media players: %s", error.getMessage().c_str());
        }
    }

    void MediaController::Play()
    {
        // Only one thread may use the D-Bus connection and the player list at a time.
        std::lock_guard lock{_mutex};

        if (!Connect())
            return;

        for (const auto& playerName : _pausedPlayers)
        {
            try
            {
                auto playerProxy = sdbus::createProxy(*_sessionBus, playerName, MPRIS_PATH);
                std::string playbackStatus = playerProxy->getProperty("PlaybackStatus")
                    .onInterface(MPRIS_PLAYER_INTERFACE);

                // Do not start a player that the user stopped or resumed manually.
                if (playbackStatus != "Paused")
                    continue;

                playerProxy->callMethod("Play").onInterface(MPRIS_PLAYER_INTERFACE);
                Logger::Debug("Media player resumed: %s", playerName.c_str());
            }
            catch (const sdbus::Error& error)
            {
                Logger::Warn("Failed to resume media player %s: %s", playerName.c_str(), error.getMessage().c_str());
            }
        }

        _pausedPlayers.clear();
    }
}
