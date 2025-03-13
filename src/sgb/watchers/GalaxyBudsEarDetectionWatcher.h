#pragma once
#include "../../Event.h"
#include "../enums/GalaxyBudsModelIds.h"
#include "../enums/GalaxyBudsLegacyEarDetectionState.h"
#include "../enums/GalaxyBudsEarDetectionState.h"
#include "../structs/GalaxyBudsResponseData.h"
#include "../structs/GalaxyBudsEarDetectionStateArgs.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <optional>

namespace MagicPodsCore
{

    class GalaxyBudsEarDetectionWatcher
    {
    private:
        GalaxyBudsModelIds model;
        Event<GalaxyBudsEarDetectionStateArgs> _earDetectionStateChanged{};
        std::optional<GalaxyBudsEarDetectionStateArgs> ExtractState(unsigned char b);
        std::optional<GalaxyBudsEarDetectionStateArgs> Process_EXTENDED_STATUS_UPDATED(GalaxyBudsResponseData data);
        std::optional<GalaxyBudsEarDetectionStateArgs> Process_STATUS_UPDATED(GalaxyBudsResponseData data);

    public:
        Event<GalaxyBudsEarDetectionStateArgs> &GetEarDetectionStateChangedEvent()
        {
            return _earDetectionStateChanged;
        }

        explicit GalaxyBudsEarDetectionWatcher(GalaxyBudsModelIds model) : model(model) {}

        void ProcessResponse(GalaxyBudsResponseData data);
    };
}