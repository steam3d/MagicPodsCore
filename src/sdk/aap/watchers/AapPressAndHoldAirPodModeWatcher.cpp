// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPressAndHoldAirPodModeWatcher.h"

#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapPressAndHoldAirPodModeWatcher::AapPressAndHoldAirPodModeWatcher() : AapWatcher{"AapPressAndHoldAirPodModeWatcher"}
    {
    }

    void AapPressAndHoldAirPodModeWatcher::ProcessResponse(const std::vector<unsigned char>& data)
    {
        if (data.size() < 11)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings) ||
            data[6] != static_cast<unsigned char>(AapCmdSettings::PressAndHoldAirPodMode))
        {
            return;
        }

        // Reject the whole state if any mode is invalid so it cannot be reused in a write.
        const auto state = AapPressAndHoldAirPodModeState::Create(
            data[7], data[8], data[9], data[10]);
        if (!state.has_value())
            return;

        const auto& active = state->GetActive();
        const auto& alternative = state->GetAlternative();
        Logger::Debug("%s: right=%s, left=%s, alternativeRight=%s, alternativeLeft=%s",
            _tag.c_str(),
            AapPressAndHoldAirPodModeToString(active[0]).c_str(),
            AapPressAndHoldAirPodModeToString(active[1]).c_str(),
            AapPressAndHoldAirPodModeToString(alternative[0]).c_str(),
            AapPressAndHoldAirPodModeToString(alternative[1]).c_str());
        _event.FireEvent(state.value());
    }
}
