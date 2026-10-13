// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapPressAndHoldDurationMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001800000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 18 00 00 00 00 Default
    // 04 00 04 00 09 00 18 01 00 00 00 Shorter
    // 04 00 04 00 09 00 18 02 00 00 00 Shortest
    //
    // 7 byte:
    // 00 - Default
    // 01 - Shorter
    // 02 - Shortest
    // ---------------------------------------------------------------------------
    class AapPressAndHoldDurationWatcher: public AapWatcher
    {
    private:
        Event<AapPressAndHoldDurationMode> _event{};

    public:
        AapPressAndHoldDurationWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapPressAndHoldDurationMode> &GetEvent()
        {
            return _event;
        }
    };
}
