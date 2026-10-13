// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/enums/AapStemControlState.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040019000602
    //
    // 0  1  2  3  4  5  6  7
    //                   p  b
    // 04 00 04 00 19 00 06 02 Double press, right AirPod
    //
    // 6 byte:
    // 05 - Single press
    // 06 - Double press
    // 07 - Triple press
    // 08 - Long press
    //
    // 7 byte:
    // 01 - Left AirPod
    // 02 - Right AirPod
    // ---------------------------------------------------------------------------
    class AapStemControlStateWatcher : public AapWatcher
    {
    private:
        Event<AapStemControlState> _event{};

    public:
        AapStemControlStateWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapStemControlState> &GetEvent()
        {
            return _event;
        }
    };
}
