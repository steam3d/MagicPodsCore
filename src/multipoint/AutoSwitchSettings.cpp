// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// settings

#include "AutoSwitchSettings.h"

namespace MagicPodsCore
{
    namespace
    {
        bool GetBool(SettingsService& settingsService, const std::string& container,
                     const std::string& name, bool fallback)
        {
            if (auto node = settingsService.GetSetting(container, name).as_boolean())
            {
                return node->get();
            }
            return fallback;
        }

        int GetInt(SettingsService& settingsService, const std::string& container,
                   const std::string& name, int fallback)
        {
            if (auto node = settingsService.GetSetting(container, name).as_integer())
            {
                return static_cast<int>(node->get());
            }
            return fallback;
        }
    }

    AutoSwitchSettings AutoSwitchSettings::Load(const std::string& container, SettingsService& settingsService)
    {
        AutoSwitchSettings out{};
        out.enabled = GetBool(settingsService, container, "sgb_auto_switch", out.enabled);
        out.takeover = GetBool(settingsService, container, "sgb_auto_switch_takeover", out.takeover);
        out.focusLock = GetBool(settingsService, container, "sgb_auto_switch_focus_lock", out.focusLock);
        out.idleTimeoutSec = GetInt(settingsService, container, "sgb_auto_switch_idle_timeout_sec", out.idleTimeoutSec);

        if (out.idleTimeoutSec < 0)
        {
            out.idleTimeoutSec = 0;
        }
        return out;
    }
}
