// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// settings

#include "AutoSwitchSettings.h"

namespace MagicPodsCore
{
    namespace
    {
        bool GetBool(SettingsService& settingsService, const std::string& name, bool fallback)
        {
            if (auto node = settingsService.GetSetting(AutoSwitchSettings::CONTAINER, name).as_boolean())
            {
                return node->get();
            }
            return fallback;
        }

        int GetInt(SettingsService& settingsService, const std::string& name, int fallback)
        {
            if (auto node = settingsService.GetSetting(AutoSwitchSettings::CONTAINER, name).as_integer())
            {
                return static_cast<int>(node->get());
            }
            return fallback;
        }
    }

    AutoSwitchSettings AutoSwitchSettings::Load(SettingsService& settingsService)
    {
        AutoSwitchSettings out{};
        out.enabled = GetBool(settingsService, "enabled", out.enabled);
        out.idleTimeoutSec = GetInt(settingsService, "idle_timeout_sec", out.idleTimeoutSec);
        out.claimCooldownSec = GetInt(settingsService, "claim_cooldown_sec", out.claimCooldownSec);
        out.stateReadIntervalMs = GetInt(settingsService, "state_read_interval_ms", out.stateReadIntervalMs);
        out.takeover = GetBool(settingsService, "takeover", out.takeover);
        out.requireGameRunning = GetBool(settingsService, "require_game_running", out.requireGameRunning);

        if (out.idleTimeoutSec < 0)
        {
            out.idleTimeoutSec = 0;
        }
        if (out.claimCooldownSec < 1)
        {
            out.claimCooldownSec = 1;
        }
        // The half-open watchdog reads the interval as "three silent intervals = dead link";
        // anything below a few seconds would rebuild the link over nothing.
        if (out.stateReadIntervalMs < 5000)
        {
            out.stateReadIntervalMs = 5000;
        }
        return out;
    }
}
