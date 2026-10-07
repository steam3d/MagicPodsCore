// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapEarDetectionWatcher.h"
#include "sdk/aap/enums/AapCmd.h"

namespace MagicPodsCore
{
    AapEarDetectionWatcher::AapEarDetectionWatcher() : AapWatcher{"AapEarDetectionWatcher"}
    {
    }

    void AapEarDetectionWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 8)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::EarDetection))
            return;

        if (!isValidAapEarDetectionState(data[6]))
            return;

        AapEarDetectionState earOne = static_cast<AapEarDetectionState>(data[6]);

        if (!isValidAapEarDetectionState(data[7]))
            return;

        AapEarDetectionState earTwo = static_cast<AapEarDetectionState>(data[7]);

        AapEarDetectionArgs state;
        state.EarOne = earOne;
        state.EarTwo = earTwo;

        Logger::Debug("%s: earOne: %s earTwo: %s", _tag.c_str(), AapEarDetectionStateToString(earOne).c_str(), AapEarDetectionStateToString(earTwo).c_str());
        _event.FireEvent(state);
    }
}