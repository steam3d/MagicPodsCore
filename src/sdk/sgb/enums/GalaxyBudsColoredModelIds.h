// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <cstdint>
#include <array>

namespace MagicPodsCore {

    enum class GalaxyBudsColoredModelIds: unsigned short {
        Unknown = 0,

        //Buds
        GalaxyBuds = 257,
        GalaxyBudsUnknown = 14336, // NOTE: Unknown device id mentioned by Samsung's device API

        //BudsPlus
        GalaxyBudsPlusBlue = 258,
        GalaxyBudsPlusPink = 259,
        GalaxyBudsPlusBlack = 260,
        GalaxyBudsPlusWhite = 261,
        GalaxyBudsPlusThomBrown = 262,
        GalaxyBudsPlusRed = 263,
        GalaxyBudsPlusDeepBlue = 264,
        GalaxyBudsPlusOlympic = 265, // NOTE: Unreleased color
        GalaxyBudsPlusPurple = 266,

        //BudsLive
        GalaxyBudsLiveBlack = 278,
        GalaxyBudsLiveWhite = 279,
        GalaxyBudsLiveBronze = 280,
        GalaxyBudsLiveRed = 281,
        GalaxyBudsLiveBlue = 282,
        GalaxyBudsLiveThomBrown = 283,
        GalaxyBudsLiveGrey = 284,

        //BudsPro
        GalaxyBudsProBlack = 298,
        GalaxyBudsProSilver = 299,
        GalaxyBudsProViolet = 300,
        GalaxyBudsProWhite = 301,

        //Buds2
        GalaxyBuds2White = 313,
        GalaxyBuds2Black = 314,
        GalaxyBuds2Yellow = 315, // NOTE: Unreleased color
        GalaxyBuds2Green = 316,
        GalaxyBuds2Violet = 317,
        GalaxyBuds2ThomBrown = 318,
        GalaxyBuds2MaisonKitsune = 319,
        GalaxyBuds2AbsoluteBlack = 320,
        GalaxyBuds2Grey = 321,
        GalaxyBuds2Unknown = 14337, // NOTE: Unknown device id mentioned by Samsung's device API

        //Buds2Pro
        GalaxyBuds2ProGrey = 326,
        GalaxyBuds2ProWhite = 327,
        GalaxyBuds2ProViolet = 328,

        //BudsFe
        GalaxyBudsFeGraphite = 330,
        GalaxyBudsFeWhite = 331,

        //Buds3
        GalaxyBuds3Silver = 333,
        GalaxyBuds3White = 334,

        //Buds3Pro
        GalaxyBuds3ProSilver = 340,
        GalaxyBuds3ProWhite = 341
    };

    inline constexpr static std::array<unsigned short, 41> GalaxyBudsModelIdsArray = {
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsUnknown),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusBlue),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusPink),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusBlack),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusWhite),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusThomBrown),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusRed),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusDeepBlue),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusOlympic),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsPlusPurple),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsLiveBlack),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsLiveWhite),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsLiveBronze),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsLiveRed),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsLiveBlue),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsLiveThomBrown),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsLiveGrey),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsProBlack),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsProSilver),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsProViolet),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsProWhite),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2White),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2Black),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2Yellow),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2Green),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2Violet),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2ThomBrown),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2MaisonKitsune),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2AbsoluteBlack),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2Grey),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2Unknown),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2ProGrey),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2ProWhite),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds2ProViolet),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsFeGraphite),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBudsFeWhite),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds3Silver),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds3White),

        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds3ProSilver),
        static_cast<unsigned short>(GalaxyBudsColoredModelIds::GalaxyBuds3ProWhite)
    };
}