// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/enums/AapHearingProtectionMode.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009003700000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 37 00 00 00 00 Unknown
    // 04 00 04 00 09 00 37 01 00 00 00 On
    // 04 00 04 00 09 00 37 02 00 00 00 Off
    //
    // 7 byte:
    // 00 - Unknown; currently exposed as disabled by the capability
    // 01 - On
    // 02 - Off
    // Reports whether hearing protection is enabled.
    // ---------------------------------------------------------------------------
    class AapHearingProtectionWatcher : public AapWatcher
    {
    private:
        Event<AapHearingProtectionMode> _event{};

    public:
        AapHearingProtectionWatcher();
        void ProcessResponse(const std::vector<unsigned char>& data) override;

        Event<AapHearingProtectionMode>& GetEvent()
        {
            return _event;
        }
    };
}
