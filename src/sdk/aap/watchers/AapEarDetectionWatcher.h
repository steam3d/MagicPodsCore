// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/structs/AapEarDetectionArgs.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 0400040006000001
    //
    // 0  1  2  3  4  5  6  7
    //                   1  2
    // 04 00 04 00 06 00 00 01 First in ear, second out of ear
    //
    // 6 byte:
    // 00 - First AirPod is in ear
    // 01 - First AirPod is out of ear
    // 02 - First AirPod is in case
    // 03 - First AirPod is in closed case, needs testing
    //
    // 7 byte:
    // 00 - Second AirPod is in ear
    // 01 - Second AirPod is out of ear
    // 02 - Second AirPod is in case
    // 03 - Second AirPod is in closed case, needs testing
    // ---------------------------------------------------------------------------
    class AapEarDetectionWatcher : public AapWatcher
    {
    private:
        Event<AapEarDetectionArgs> _event{};

    public:
        AapEarDetectionWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapEarDetectionArgs> &GetEvent()
        {
            return _event;
        }
    };

}
