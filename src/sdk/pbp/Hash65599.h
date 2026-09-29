// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string_view>
#include <cstdint>

namespace MagicPodsCore
{
    // Pigweed RPC derives the service and method ids of a call from the hash of their names.
    // The implementation is constexpr so that ids can be built at compile time.
    class Hash65599
    {
    private:
        static inline constexpr uint32_t hashConst = 65599;

    public:
        static constexpr uint32_t Hash(std::string_view id)
        {
            uint32_t hash = static_cast<uint32_t>(id.size());
            uint32_t coef = hashConst;

            for (char c : id)
            {
                hash += coef * static_cast<uint32_t>(static_cast<unsigned char>(c));
                coef *= hashConst;
            }

            return hash;
        }
    };
}
