// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapHearingProtectionWatcher.h"

#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapHearingProtectionWatcher::AapHearingProtectionWatcher() : AapWatcher{"AapHearingProtectionWatcher"}
    {
    }

    void AapHearingProtectionWatcher::ProcessResponse(const std::vector<unsigned char>& data)
    {
        if (data.size() < 8)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::HearingProtection))
            return;

        if (!IsValidAapHearingProtectionMode(data[7]))
            return;

        const auto mode = static_cast<AapHearingProtectionMode>(data[7]);
        Logger::Debug("%s: %s", _tag.c_str(), AapHearingProtectionModeToString(mode).c_str());
        _event.FireEvent(mode);
    }
}
