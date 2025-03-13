#pragma once
#include "../../Event.h"
#include "../enums/GalaxyBudsModelIds.h"
#include "../enums/GalaxyBudsMsgIds.h"
#include "../enums/GalaxyBudsAnc.h"
#include "../structs/GalaxyBudsResponseData.h"
#include <optional>
#include "vector"

namespace MagicPodsCore
{

    class GalaxyBudsAncWatcher
    {
    private:
        GalaxyBudsModelIds model;
        Event<GalaxyBudsAnc> _ancChanged{};
        std::optional<GalaxyBudsAnc> ExtractState(unsigned char b);
        std::optional<GalaxyBudsAnc> Process_EXTENDED_STATUS_UPDATED(GalaxyBudsResponseData data);
        std::optional<GalaxyBudsAnc> Process_NOISE_CONTROLS_UPDATE(GalaxyBudsResponseData data);
        std::optional<GalaxyBudsAnc> Process_UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT(GalaxyBudsResponseData data);

    public:
        Event<GalaxyBudsAnc> &GetBatteryChangedEvent()
        {
            return _ancChanged;
        }

        explicit GalaxyBudsAncWatcher(GalaxyBudsModelIds model) : model(model) {}

        void ProcessResponse(GalaxyBudsResponseData data);
        static bool IsSupport(GalaxyBudsModelIds model);
        static std::vector<GalaxyBudsAnc> GetAncModesFor(GalaxyBudsModelIds model);
    };

}