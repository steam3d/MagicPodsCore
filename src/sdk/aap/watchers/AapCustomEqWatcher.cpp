// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapCustomEqWatcher.h"

#include "sdk/aap/enums/AapCmd.h"

namespace MagicPodsCore
{
    AapCustomEqWatcher::AapCustomEqWatcher() : AapWatcher{"AapCustomEqWatcher"}
    {
    }

    void AapCustomEqWatcher::ProcessResponse(const std::vector<unsigned char>& data)
    {
        if (data.size() < 13)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::CustomEq))
            return;

        const unsigned short payloadLength = static_cast<unsigned short>(data[6]) |
                                             static_cast<unsigned short>(data[7]) << 8;
        if (payloadLength != 5 || data[8] != 0x01)
            return;

        if (!isValidAapCustomEqState(data[9]) || data[10] > 100 || data[11] > 100 || data[12] > 100)
            return;

        AapCustomEq mode{
            static_cast<AapCustomEqState>(data[9]),
            data[10],
            data[11],
            data[12]
        };

        Logger::Debug("%s: %s", _tag.c_str(), mode.ToString().c_str());
        _event.FireEvent(mode);
    }
}
