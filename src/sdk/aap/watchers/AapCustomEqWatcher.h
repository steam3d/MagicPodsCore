// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/structs/AapCustomEq.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 04000400630005000102323232
    //
    // 0  1  2  3  4  5  6  7  8  9  10 11 12
    //                            s  l  m  h
    // 04 00 04 00 63 00 05 00 01 02 32 32 32 Enabled, 50/50/50
    //
    // 6-7 bytes:
    // 05 00 - Payload length
    //
    // 8 byte:
    // 01 - Equalizer state packet
    //
    // 9 byte:
    // 01 - Disabled
    // 02 - Enabled
    //
    // 10 byte:
    // 00-64 - Low frequency level from 0 to 100
    //
    // 11 byte:
    // 00-64 - Mid frequency level from 0 to 100
    //
    // 12 byte:
    // 00-64 - High frequency level from 0 to 100
    // ---------------------------------------------------------------------------
    class AapCustomEqWatcher : public AapWatcher
    {
    private:
        Event<AapCustomEq> _event{};

    public:
        AapCustomEqWatcher();
        void ProcessResponse(const std::vector<unsigned char>& data) override;

        Event<AapCustomEq>& GetEvent()
        {
            return _event;
        }
    };
}
