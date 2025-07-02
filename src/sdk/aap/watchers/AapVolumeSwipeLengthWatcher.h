// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapVolumeSwipeLengthMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
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