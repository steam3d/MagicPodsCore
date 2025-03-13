#pragma once
#include "GalaxyBudsModelIds.h"
#include "GalaxyBudsLegacyEarDetectionState.h"
#include "GalaxyBudsEarDetectionState.h"
#include "GalaxyBudsResponseData.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <optional>
#include "../Event.h"


namespace MagicPodsCore{
    struct GalaxyBudsEarDetectionStateArgs{
        GalaxyBudsEarDetectionState Left;
        GalaxyBudsEarDetectionState Right;

        GalaxyBudsEarDetectionStateArgs(GalaxyBudsEarDetectionState left, GalaxyBudsEarDetectionState right)
        : Left(left), Right(right) {}

        std::string ToString() const {
            std::ostringstream os;
            os << "Left: " << static_cast<int>(Left) << " Right:" << static_cast<int>(Right);
            
            return os.str();
        }

        static bool AreStatesEqual(GalaxyBudsEarDetectionStateArgs state1, GalaxyBudsEarDetectionStateArgs state2)
        {            
            return state1.Left == state2.Left && state1.Right == state2.Right;
        }
    };

    class GalaxyBudsEarDetectionWatcher
    {
        private:
            GalaxyBudsModelIds model;
            Event<GalaxyBudsEarDetectionStateArgs> _earDetectionStateChanged{};
            std::optional<GalaxyBudsEarDetectionStateArgs> ExtractState(unsigned char b);
            std::optional<GalaxyBudsEarDetectionStateArgs> Process_EXTENDED_STATUS_UPDATED(GalaxyBudsResponseData data);
            std::optional<GalaxyBudsEarDetectionStateArgs> Process_STATUS_UPDATED(GalaxyBudsResponseData data);

        public:
            Event<GalaxyBudsEarDetectionStateArgs>& GetEarDetectionStateChangedEvent() {
                return _earDetectionStateChanged;
            }

            explicit GalaxyBudsEarDetectionWatcher(GalaxyBudsModelIds model): model(model) {}

            void ProcessResponse(GalaxyBudsResponseData data);
    };
}