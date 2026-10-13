// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapAncMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009000d02000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 0d 01 00 00 00 Off
    // 04 00 04 00 09 00 0d 02 00 00 00 Noise cancellation
    // 04 00 04 00 09 00 0d 03 00 00 00 Transparency
    // 04 00 04 00 09 00 0d 04 00 00 00 Adaptive
    //
    // 7 byte:
    // 01 - Off
    // 02 - Noise cancellation
    // 03 - Transparency
    // 04 - Adaptive
    // ---------------------------------------------------------------------------
    class AapAncWatcher : public AapWatcher
    {
    private:
        Event<AapAncMode> _event{};

    public:
        AapAncWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapAncMode> &GetEvent()
        {
            return _event;
        }
    };

}
