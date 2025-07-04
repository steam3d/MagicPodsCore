// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapMuteMicrophoneEndCall.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    class AapMuteMicrophoneEndCallWatcher: public AapWatcher
    {
    private:
        Event<AapMuteMicrophoneEndCallMode> _event{};

    public:
        AapMuteMicrophoneEndCallWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapMuteMicrophoneEndCallMode> &GetEvent()
        {
            return _event;
        }
    };
}