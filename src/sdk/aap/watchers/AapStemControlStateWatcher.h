// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/enums/AapStemControlState.h"

namespace MagicPodsCore
{
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
