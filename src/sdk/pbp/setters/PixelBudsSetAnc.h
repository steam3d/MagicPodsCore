// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "sdk/pbp/enums/PixelBudsAncState.h"
#include <vector>
#include <cstdint>

namespace MagicPodsCore
{

    // maestro_pw.Maestro/WriteSetting with a WriteSettingMsg holding SettingValue.current_ancr_state
    class PixelBudsSetAnc
    {
    public:
        const uint32_t MethodId;
        const std::vector<unsigned char> Payload;
        explicit PixelBudsSetAnc(PixelBudsAncState state);
    };
}
