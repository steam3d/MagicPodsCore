// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/structs/AapPrivateKeysArgs.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    class AapPrivateKeysWatcher: public AapWatcher
    {
    private:
        Event<AapPrivateKeysArgs> _event{};

    public:
        AapPrivateKeysWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapPrivateKeysArgs> &GetEvent()
        {
            return _event;
        }
    };
}