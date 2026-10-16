// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/structs/AapPressAndHoldAirPodModeState.h"

namespace MagicPodsCore
{
    class AapPressAndHoldAirPodModeWatcher : public AapWatcher
    {
    private:
        Event<AapPressAndHoldAirPodModeState> _event{};

    public:
        AapPressAndHoldAirPodModeWatcher();
        void ProcessResponse(const std::vector<unsigned char>& data) override;

        Event<AapPressAndHoldAirPodModeState>& GetEvent()
        {
            return _event;
        }
    };
}
