// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/structs/AapCustomEq.h"

namespace MagicPodsCore
{
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
