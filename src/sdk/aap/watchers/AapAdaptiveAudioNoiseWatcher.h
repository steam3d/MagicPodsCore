// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    class AapAdaptiveAudioNoiseWatcher: public AapWatcher
    {
    private:
        Event<unsigned char> _event{};

    public:
        AapAdaptiveAudioNoiseWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<unsigned char> &GetEvent()
        {
            return _event;
        }
    };
}