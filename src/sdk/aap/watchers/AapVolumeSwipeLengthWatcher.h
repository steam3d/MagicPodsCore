// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapVolumeSwipeLengthMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009002300000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 23 00 00 00 00 Default
    // 04 00 04 00 09 00 23 01 00 00 00 Longer
    // 04 00 04 00 09 00 23 02 00 00 00 Longest
    //
    // 7 byte:
    // 00 - Default
    // 01 - Longer
    // 02 - Longest
    // ---------------------------------------------------------------------------
    class AapVolumeSwipeLengthWatcher: public AapWatcher
    {
    private:
        Event<AapVolumeSwipeLengthMode> _event{};

    public:
        AapVolumeSwipeLengthWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapVolumeSwipeLengthMode> &GetEvent()
        {
            return _event;
        }
    };
}
