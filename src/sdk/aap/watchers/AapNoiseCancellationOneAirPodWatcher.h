// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapNoiseCancellationOneAirPodMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001b01000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 1b 01 00 00 00 On
    // 04 00 04 00 09 00 1b 02 00 00 00 Off
    //
    // 7 byte:
    // 01 - On
    // 02 - Off
    // ---------------------------------------------------------------------------
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
