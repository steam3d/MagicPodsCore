// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/structs/AapEarDetectionArgs.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    class AapEarDetectionWatcher : public AapWatcher
    {
    private:
        Event<AapEarDetectionArgs> _event{};

    public:
        AapEarDetectionWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapEarDetectionArgs> &GetEvent()
        {
            return _event;
        }
    };

}