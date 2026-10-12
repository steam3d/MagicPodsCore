// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapAncOffMode.h"

namespace MagicPodsCore
{
    class AapSetAncOffMode : public AapRequest
    {
    private:
        AapAncOffMode _mode;

    public:
        explicit AapSetAncOffMode(AapAncOffMode mode);
        std::vector<unsigned char> Request() const override;
    };
}
