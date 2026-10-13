// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040009001f32000000
    //
    // 0  1  2  3  4  5  6  7  8  9  10
    //                      m
    // 04 00 04 00 09 00 1f 32 00 00 00 50%
    //
    // 7 byte:
    // 0f-64 - Volume from 15 to 100
    // ---------------------------------------------------------------------------
    class AapToneVolumeWatcher: public AapWatcher
    {
    private:
        Event<unsigned char> _event{};

    public:
        AapToneVolumeWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<unsigned char> &GetEvent()
        {
            return _event;
        }
    };
}
