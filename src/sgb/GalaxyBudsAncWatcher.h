#pragma once
#include "GalaxyBudsModelIds.h"
#include "GalaxyBudsResponseData.h"
#include "GalaxyBudsMsgIds.h"
#include "GalaxyBudsAnc.h"
#include <optional>
#include "vector"
#include "../Event.h"

namespace MagicPodsCore{
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
            Event<GalaxyBudsAnc>& GetBatteryChangedEvent() {
                return _ancChanged;
            }

            explicit GalaxyBudsAncWatcher(GalaxyBudsModelIds model): model(model) {}

            void ProcessResponse(GalaxyBudsResponseData data);
            static bool IsSupport(GalaxyBudsModelIds model);
            static std::vector<GalaxyBudsAnc> GetAncModesFor(GalaxyBudsModelIds model);
    };

}