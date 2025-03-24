#pragma once
#include "Event.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
#include "sdk/sgb/structs/GalaxyBudsEarDetectionStateArgs.h"
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
        std::optional<GalaxyBudsEarDetectionStateArgs> Process_EXTENDED_STATUS_UPDATED(const GalaxyBudsResponseData &data);
        std::optional<GalaxyBudsEarDetectionStateArgs> Process_STATUS_UPDATED(const GalaxyBudsResponseData &data);

    public:
        Event<GalaxyBudsEarDetectionStateArgs> &GetEarDetectionStateChangedEvent()
        {
            return _earDetectionStateChanged;
        }

        explicit GalaxyBudsEarDetectionWatcher(GalaxyBudsModelIds model) : model(model) {}

        void ProcessResponse(const GalaxyBudsResponseData &data);
    };
}