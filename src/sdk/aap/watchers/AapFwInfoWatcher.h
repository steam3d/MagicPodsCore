// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/structs/AapFwInfo.h"

namespace MagicPodsCore
{
    class AapFwInfoWatcher : public AapWatcher
    {
    private:
        static constexpr size_t ServiceFieldOffset = 9;
        static constexpr size_t FwInfoFieldCount = 11;

        Event<AapFwInfo> _event{};

    public:
        AapFwInfoWatcher();
        void ProcessResponse(const std::vector<unsigned char>& data) override;

        Event<AapFwInfo>& GetEvent()
        {
            return _event;
        }
    };
}
