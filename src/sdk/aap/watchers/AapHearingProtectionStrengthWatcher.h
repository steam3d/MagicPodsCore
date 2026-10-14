// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009003852000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      s
    // 04 00 04 00 09 00 38 52 00 00 00 82
    //
    // 7 byte:
    // Raw hearing protection strength reported by the headphones.
    // valid range are unknown.    
    // ---------------------------------------------------------------------------
    class AapHearingProtectionStrengthWatcher : public AapWatcher
    {
    private:
        Event<unsigned char> _event{};

    public:
        AapHearingProtectionStrengthWatcher();
        void ProcessResponse(const std::vector<unsigned char>& data) override;

        Event<unsigned char>& GetEvent()
        {
            return _event;
        }
    };
}
