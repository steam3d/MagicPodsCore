// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapNoiseCancellationOneAirPodMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    class AapNoiseCancellationOneAirPodWatcher: public AapWatcher
    {
    private:
        Event<AapNoiseCancellationOneAirPodMode> _event{};

    public:
        AapNoiseCancellationOneAirPodWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapNoiseCancellationOneAirPodMode> &GetEvent()
        {
            return _event;
        }
    };
}