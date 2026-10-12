// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapRequest.h"
#include "sdk/aap/enums/AapDigitalCrownMode.h"

namespace MagicPodsCore
{
    class AapSetDigitalCrown : public AapRequest
    {
    private:
        AapDigitalCrownMode _mode;

    public:
        explicit AapSetDigitalCrown(AapDigitalCrownMode mode);
        std::vector<unsigned char> Request() const override;
    };
}
