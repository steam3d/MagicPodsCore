// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "Event.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "sdk/sgb/enums/GalaxyBudsAnc.h"
#include "sdk/sgb/structs/GalaxyBudsResponseData.h"
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
        std::optional<GalaxyBudsAnc> Process_EXTENDED_STATUS_UPDATED(const GalaxyBudsResponseData &data);
        std::optional<GalaxyBudsAnc> Process_NOISE_CONTROLS_UPDATE(const GalaxyBudsResponseData &data);
        std::optional<GalaxyBudsAnc> Process_UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT(const GalaxyBudsResponseData &data);

    public:
        Event<GalaxyBudsAnc> &GetAncChangedEvent()
        {
            return _ancChanged;
        }

        explicit GalaxyBudsAncWatcher(GalaxyBudsModelIds model) : model(model) {}

        void ProcessResponse(const GalaxyBudsResponseData &data);
        static bool IsSupport(GalaxyBudsModelIds model);
        static std::vector<GalaxyBudsAnc> GetAncModesFor(GalaxyBudsModelIds model);
    };

}