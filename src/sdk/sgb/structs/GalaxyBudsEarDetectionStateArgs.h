// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/sgb/enums/GalaxyBudsEarDetectionState.h"

namespace MagicPodsCore
{

    struct GalaxyBudsEarDetectionStateArgs
    {
        GalaxyBudsEarDetectionState Left;
        GalaxyBudsEarDetectionState Right;

         GalaxyBudsEarDetectionStateArgs() = default;

        GalaxyBudsEarDetectionStateArgs(GalaxyBudsEarDetectionState left, GalaxyBudsEarDetectionState right)
            : Left(left), Right(right) {}

        std::string ToString() const
        {
            std::ostringstream os;
            os << "Left: " << static_cast<int>(Left) << " Right:" << static_cast<int>(Right);

            return os.str();
        }

        bool AreStatesEqual(GalaxyBudsEarDetectionStateArgs state)
        {
            return Left == state.Left && Right == state.Right;
        }        
    };
}