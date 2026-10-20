// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore
{

    enum class PixelBudsModelIds : unsigned short
    {
        Unknown = 0,
        // PixelBuds1, // TODO: Product ID is required.
        // PixelBuds2, // TODO: Product ID is required.
        PixelBudsASeries = 12291, // Uses GFPS Message Stream instead of Maestro.
        PixelBudsPro = 12292,
        PixelBudsPro2 = 12293,
        PixelBuds2a = 12294
    };
}
