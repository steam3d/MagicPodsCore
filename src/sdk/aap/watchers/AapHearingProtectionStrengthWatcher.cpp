// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapHearingProtectionStrengthWatcher.h"

#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapHearingProtectionStrengthWatcher::AapHearingProtectionStrengthWatcher() : AapWatcher{"AapHearingProtectionStrengthWatcher"}
    {
    }

    void AapHearingProtectionStrengthWatcher::ProcessResponse(const std::vector<unsigned char>& data)
    {
        if (data.size() < 8)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::HearingProtectionStrength))
            return;

        const unsigned char strength = data[7];
        Logger::Debug("%s: %d", _tag.c_str(), strength);
        _event.FireEvent(strength);
    }
}
