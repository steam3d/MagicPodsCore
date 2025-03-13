#pragma once

#include "../enums/GalaxyBudsEarDetectionState.h"

namespace MagicPodsCore
{

    struct GalaxyBudsEarDetectionStateArgs
    {
        GalaxyBudsEarDetectionState Left;
        GalaxyBudsEarDetectionState Right;

        GalaxyBudsEarDetectionStateArgs(GalaxyBudsEarDetectionState left, GalaxyBudsEarDetectionState right)
            : Left(left), Right(right) {}

        std::string ToString() const
        {
            std::ostringstream os;
            os << "Left: " << static_cast<int>(Left) << " Right:" << static_cast<int>(Right);

            return os.str();
        }

        static bool AreStatesEqual(GalaxyBudsEarDetectionStateArgs state1, GalaxyBudsEarDetectionStateArgs state2)
        {
            return state1.Left == state2.Left && state1.Right == state2.Right;
        }
    };
}