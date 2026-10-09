// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapCustomEqState.h"
#include <sstream>

namespace MagicPodsCore
{
    struct AapCustomEq
    {
        AapCustomEqState State{AapCustomEqState::Disabled};
        unsigned char Low{50};
        unsigned char Mid{50};
        unsigned char High{50};

        std::string ToString() const
        {
            std::ostringstream os;
            os << "CustomEq: " << AapCustomEqStateToString(State)
               << " Low: " << static_cast<int>(Low)
               << " Mid: " << static_cast<int>(Mid)
               << " High: " << static_cast<int>(High);

            return os.str();
        }

        bool AreStatesEqual(const AapCustomEq& state) const
        {
            return State == state.State &&
                   Low == state.Low &&
                   Mid == state.Mid &&
                   High == state.High;
        }
    };
}
