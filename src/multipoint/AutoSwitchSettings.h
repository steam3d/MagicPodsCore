// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// settings

#pragma once

#include "settings/SettingsService.h"

#include <cstdint>
#include <memory>
#include <string>

namespace MagicPodsCore
{
    // Everything the Auto Switch policy can be told to do, in the TOML container
    // "auto_switch". All of it is off by default: the feature only runs when the user
    // turns it on, and the class write stays off unless the user asks for it, because
    // reporting devType 02 costs something real (see the notes, 14.22).
    struct AutoSwitchSettings
    {
        static constexpr const char* CONTAINER = "auto_switch";

        // Master switch. Off by default.
        bool enabled = false;
        // Fallback wait before we take the focus back when the firmware's own release push
        // never arrived. Normally the buds push the grant-back (state 0x81 / output -> 0x00)
        // 6-10 s after the other host goes silent, and that push is the trigger (20.2);
        // this timer only covers the case where it stays missing.
        int idleTimeoutSec = 5;
        // Minimum gap between two 0x71 focus claims.
        int claimCooldownSec = 10;
        // Keepalive cadence of the 44 00 02 ALL_CURRENT_STATES read. The PC client hammers
        // this exact read all day with zero side effects (17.13-M F); one write buys the
        // state snapshot, keeps the link out of idle eviction, and answers double as the
        // liveness proof the half-open watchdog needs.
        int stateReadIntervalMs = 45000;
        // Claim the focus back when OUR audio goes onto the buds while the other host holds
        // it (the mirror of the official Auto Switch, 20.9: without a claim the buds kill
        // the unauthorised stream with an AVRCP pause in ~1 s). Runs under "enabled".
        bool takeover = true;
        // Only take the focus back while a Steam game is running (rule B gate).
        bool requireGameRunning = true;

        static AutoSwitchSettings Load(SettingsService& settingsService);
    };
}
