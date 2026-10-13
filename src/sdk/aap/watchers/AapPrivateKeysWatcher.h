// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "sdk/aap/structs/AapPrivateKeysArgs.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 04 00 04 00 31 00 cc [key records]
    //
    // 4 byte:
    // 31 - Private keys response
    //
    // 6 byte:
    // Number of key records
    //
    // Each key record:
    // 0 byte:
    // 01 - Identity Resolving Key (IRK)
    // 04 - Encryption key (ENC)
    //
    // 2 byte:
    // Key length
    //
    // 4 byte:
    // Start of key data
    // ---------------------------------------------------------------------------
    class AapPrivateKeysWatcher: public AapWatcher
    {
    private:
        Event<AapPrivateKeysArgs> _event{};

    public:
        AapPrivateKeysWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapPrivateKeysArgs> &GetEvent()
        {
            return _event;
        }
    };
}
