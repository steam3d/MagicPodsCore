// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "Event.h"
#include "sdk/pbp/enums/PixelBudsModelIds.h"
#include "sdk/pbp/enums/PixelBudsAncState.h"
#include "sdk/pbp/PwRpcPacket.h"
#include <optional>
#include "vector"

namespace MagicPodsCore
{

    class PixelBudsAncWatcher
    {
    private:
        PixelBudsModelIds model;
        Event<PixelBudsAncState> _ancChanged{};
        std::optional<PixelBudsAncState> ExtractState(const std::vector<unsigned char> &settingsRsp);

    public:
        Event<PixelBudsAncState> &GetAncChangedEvent()
        {
            return _ancChanged;
        }

        explicit PixelBudsAncWatcher(PixelBudsModelIds model) : model(model) {}

        void ProcessResponse(const PwRpcPacket &packet);
        static bool IsSupport(PixelBudsModelIds model);
        static std::vector<PixelBudsAncState> GetAncModesFor(PixelBudsModelIds model);
    };

}
