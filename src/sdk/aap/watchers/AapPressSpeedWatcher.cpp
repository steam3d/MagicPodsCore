// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPressSpeedWatcher.h"
#include "sdk/aap/enums/AapCmd.h"
#include "sdk/aap/enums/AapCmdSettings.h"

namespace MagicPodsCore
{
    AapPressSpeedWatcher::AapPressSpeedWatcher() : AapWatcher{"AapPressSpeedWatcher"}
    {
    }

    void AapPressSpeedWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 10)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::Settings))
            return;

        if (data[6] != static_cast<unsigned char>(AapCmdSettings::PressSpeed))
            return;

        if (!isValidAapPressSpeedMode(data[7]))
            return;

        AapPressSpeedMode mode = static_cast<AapPressSpeedMode>(data[7]);

        Logger::Debug("%s: %s", _tag.c_str(), AapPressSpeedModeToString(mode).c_str());
        _event.FireEvent(mode);
    }
}