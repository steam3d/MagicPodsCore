// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// "is a game running" signal

#pragma once

#include <optional>

namespace MagicPodsCore
{
    // The one SteamOS signal that survives a Proton game and needs no root: Steam's reaper
    // process, "...\Steam/ubuntu12_32/reaper SteamLaunch AppId=<n> ...", exists exactly while
    // a game runs (measured on SteamOS 3.9.5, "E-D4").
    // Both PulseAudio's media.role and the STEAM_GAME environment variable were ruled out there,
    // and whether gamescope is running says nothing about a game being in the foreground.
    class SteamGamePresence
    {
    public:
        // AppId of the running game, or nullopt when no reaper is present.
        static std::optional<int> RunningAppId();

        // True when a game is running. Cached for `ttlSeconds`, because the caller asks once
        // a second and /proc does not change that fast.
        static bool IsGameRunning(int ttlSeconds = 2);
    };
}
