// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapWatcher.h"
#include "Event.h"
#include "sdk/aap/structs/AapFwInfo.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // 04 00 04 00 1d 00 xx ll ll [service] 00 [device information]
    //
    // 4 byte:
    // 1d - Firmware information
    //
    // 7-8 bytes:
    // Payload length in little-endian order
    //
    // 9 byte:
    // Start of the null-terminated service field
    //
    // Device information follows the service field as null-terminated strings:
    // name, model, manufacturer, serial, version 1, version 2, hardware revision,
    // updater identifier, left serial, right serial, version 3
    // ---------------------------------------------------------------------------
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
