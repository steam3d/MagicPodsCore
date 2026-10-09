// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapSetCustomEq.h"

#include "sdk/aap/enums/AapCmd.h"

#include <algorithm>

namespace MagicPodsCore
{
    AapSetCustomEq::AapSetCustomEq(AapCustomEq customEq) : AapRequest{"AapSetCustomEq"}, _customEq{customEq}
    {
        _customEq.Low = std::min<unsigned char>(_customEq.Low, 100);
        _customEq.Mid = std::min<unsigned char>(_customEq.Mid, 100);
        _customEq.High = std::min<unsigned char>(_customEq.High, 100);
    }

    std::vector<unsigned char> AapSetCustomEq::Request() const
    {
        return {
            0x04, 0x00, 0x04, 0x00,
            static_cast<unsigned char>(AapCmd::CustomEq), 0x00,
            0x05, 0x00, 0x01,
            static_cast<unsigned char>(_customEq.State),
            _customEq.Low, _customEq.Mid, _customEq.High
        };
    }
}
