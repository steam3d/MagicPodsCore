#pragma once
#include <cstdint>
#include <array>

namespace MagicPodsCore {

    enum class GalaxyBudsColoredModelIds: int16_t {
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

    inline constexpr static std::array<int16_t, 41> GalaxyBudsModelIdsArray = {
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsUnknown),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusBlue),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusPink),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusBlack),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusWhite),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusThomBrown),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusRed),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusDeepBlue),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusOlympic),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsPlusPurple),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsLiveBlack),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsLiveWhite),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsLiveBronze),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsLiveRed),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsLiveBlue),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsLiveThomBrown),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsLiveGrey),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsProBlack),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsProSilver),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsProViolet),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsProWhite),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2White),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2Black),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2Yellow),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2Green),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2Violet),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2ThomBrown),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2MaisonKitsune),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2AbsoluteBlack),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2Grey),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2Unknown),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2ProGrey),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2ProWhite),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2ProViolet),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsFeGraphite),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBudsFeWhite),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds3Silver),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds3White),
    
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds3ProSilver),
        static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds3ProWhite)
    };
}