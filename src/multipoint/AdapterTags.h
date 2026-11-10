// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// which group-table rows are us

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace MagicPodsCore
{
    // The device group table identifies every host by the tail of its BD_ADDR, so telling our
    // own rows apart from the other host's needs our adapter addresses; BlueZ (org.bluez
    // Adapter1) is the authority on the host's own BD_ADDR.
    class AdapterTags
    {
    public:
        // Adapter addresses as "AA:BB:CC:DD:EE:FF", from org.bluez Adapter1 objects.
        static std::vector<std::string> AdapterAddresses();

        // The same adapters as the two-byte tags the buds use (F4:AB:5C:F2:0B:8D -> 0x0B8D).
        static std::vector<uint16_t> LocalTags();
    };
}
