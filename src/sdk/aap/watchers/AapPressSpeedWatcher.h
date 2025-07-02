// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapPressSpeedMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    class AapPressSpeedWatcher: public AapWatcher
    {
    private:
        Event<AapPressSpeedMode> _event{};

    public:
        AapPressSpeedWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapPressSpeedMode> &GetEvent()
        {
            return _event;
        }
    };
}