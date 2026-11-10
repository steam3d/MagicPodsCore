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
    // User-facing Auto Switch options, stored in the per-device settings container
    // under the sgb_ prefix. Off by default: the feature only runs once the user
    // turns it on.
    struct AutoSwitchSettings
    {
        // Master switch.
        bool enabled = false;
        // Claim the focus back when our own audio goes onto the buds while the other
        // host holds it.
        bool takeover = true;
        // Focus lock: report our record as a phone-class device so the firmware
        // refuses to hand our audio focus over to another host.
        bool focusLock = false;
        // Fallback wait before the focus is taken back when the firmware's own
        // release push never arrives. The buds normally push the grant-back
        // themselves a few seconds after the other host goes silent; this timer
        // only covers the case where that push stays missing.
        int idleTimeoutSec = 5;

        // Policy parameters that are not user settings.
        // Minimum gap between two focus claims.
        int claimCooldownSec = 10;
        // Cadence of the group-table read; doubles as the SMEP link keepalive.
        int stateReadIntervalMs = 45000;

        static AutoSwitchSettings Load(const std::string& container, SettingsService& settingsService);
    };
}
