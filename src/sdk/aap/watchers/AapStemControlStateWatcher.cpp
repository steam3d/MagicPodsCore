// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapStemControlStateWatcher.h"
#include "sdk/aap/enums/AapCmd.h"

namespace MagicPodsCore
{
    AapStemControlStateWatcher::AapStemControlStateWatcher() : AapWatcher{"AapStemControlStateWatcher"}
    {
    }
    //TODO: check AirPods Max 2 response
    void AapStemControlStateWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 8)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::StemPress))
            return;

        if (!isValidAapStemControlPress(data[6]) || !isValidAapStemControlBud(data[7]))
            return;

        AapStemControlState state{
            static_cast<AapStemControlPress>(data[6]),
            static_cast<AapStemControlBud>(data[7])
        };

        Logger::Debug("%s: %s, %s", _tag.c_str(), AapStemControlPressToString(state.Press).c_str(), AapStemControlBudToString(state.Bud).c_str());
        _event.FireEvent(state);
    }
}
