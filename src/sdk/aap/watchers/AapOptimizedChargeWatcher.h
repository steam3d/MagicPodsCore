// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/enums/AapOptimizedChargeMode.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009003b02000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 3b 01 00 00 00 On
    // 04 00 04 00 09 00 3b 02 00 00 00 Off
    //
    // 7 byte:
    // 01 - On
    // 02 - Off
    // Reports whether optimized charging is enabled.
    // ---------------------------------------------------------------------------
    class AapOptimizedChargeWatcher : public AapWatcher
    {
    private:
        Event<AapOptimizedChargeMode> _event{};

    public:
        AapOptimizedChargeWatcher();
        void ProcessResponse(const std::vector<unsigned char>& data) override;

        Event<AapOptimizedChargeMode>& GetEvent()
        {
            return _event;
        }
    };
}
