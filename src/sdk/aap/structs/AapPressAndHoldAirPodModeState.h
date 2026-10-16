// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "sdk/aap/enums/AapPressAndHoldAirPodMode.h"

#include <array>
#include <optional>

namespace MagicPodsCore
{
    struct AapPressAndHoldAirPodModeState
    {
    public:
        using ModePair = std::array<AapPressAndHoldAirPodMode, 2>;

    private:
        ModePair _active;
        ModePair _alternative;

        AapPressAndHoldAirPodModeState(ModePair active, ModePair alternative);

        static bool IsVolumeMode(AapPressAndHoldAirPodMode mode);
        static bool IsStandardMode(AapPressAndHoldAirPodMode mode);
        static bool IsValidPair(const ModePair& pair);
        ModePair GetVolumeModes() const;
        ModePair GetStandardModes() const;

    public:
        static std::optional<AapPressAndHoldAirPodModeState> Create(
            unsigned char activeRight,
            unsigned char activeLeft,
            unsigned char alternativeRight,
            unsigned char alternativeLeft);

        AapPressAndHoldAirPodModeState SetRight(int value) const;
        AapPressAndHoldAirPodModeState SetLeft(int value) const;
        bool IsVolumeSupported() const;

        const ModePair& GetActive() const { return _active; }
        const ModePair& GetAlternative() const { return _alternative; }

        bool operator==(const AapPressAndHoldAirPodModeState&) const = default;
    };
}
