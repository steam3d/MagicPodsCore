// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/structs/AapCustomEq.h"

namespace MagicPodsCore
{
    class AapSetCustomEq : public AapRequest
    {
    private:
        AapCustomEq _customEq{};

    public:
        explicit AapSetCustomEq(AapCustomEq customEq);
        std::vector<unsigned char> Request() const override;
    };
}
