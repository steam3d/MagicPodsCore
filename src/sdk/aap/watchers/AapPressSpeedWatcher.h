// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapPressSpeedMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001700000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 17 00 00 00 00 Default
    // 04 00 04 00 09 00 17 01 00 00 00 Slower
    // 04 00 04 00 09 00 17 02 00 00 00 Slowest
    //
    // 7 byte:
    // 00 - Default
    // 01 - Slower
    // 02 - Slowest
    // ---------------------------------------------------------------------------
    class AapPressSpeedWatcher: public AapWatcher
    {
    private:
        Event<AapPressSpeedMode> _event{};

    public:
        AapPressSpeedWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapPressSpeedMode> &GetEvent()
        {
            return _event;
        }
    };
}
