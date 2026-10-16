// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPressAndHoldAirPodModeState.h"

namespace MagicPodsCore
{
    AapPressAndHoldAirPodModeState::AapPressAndHoldAirPodModeState(
        ModePair active,
        ModePair alternative)
        : _active{active}, _alternative{alternative}
    {
    }

    bool AapPressAndHoldAirPodModeState::IsVolumeMode(AapPressAndHoldAirPodMode mode)
    {
        return mode == AapPressAndHoldAirPodMode::VolumeUp ||
               mode == AapPressAndHoldAirPodMode::VolumeDown;
    }

    bool AapPressAndHoldAirPodModeState::IsStandardMode(AapPressAndHoldAirPodMode mode)
    {
        return mode == AapPressAndHoldAirPodMode::NoiseControl ||
               mode == AapPressAndHoldAirPodMode::Siri;
    }

    bool AapPressAndHoldAirPodModeState::IsValidPair(const ModePair& pair)
    {
        return (IsStandardMode(pair[0]) && IsStandardMode(pair[1])) ||
               (IsVolumeMode(pair[0]) && IsVolumeMode(pair[1]) && pair[0] != pair[1]);
    }

    std::optional<AapPressAndHoldAirPodModeState> AapPressAndHoldAirPodModeState::Create(
        unsigned char activeRight,
        unsigned char activeLeft,
        unsigned char alternativeRight,
        unsigned char alternativeLeft)
    {
        const ModePair active{
            static_cast<AapPressAndHoldAirPodMode>(activeRight),
            static_cast<AapPressAndHoldAirPodMode>(activeLeft)
        };
        const ModePair alternative{
            static_cast<AapPressAndHoldAirPodMode>(alternativeRight),
            static_cast<AapPressAndHoldAirPodMode>(alternativeLeft)
        };

        if (!IsValidPair(active) || !IsValidPair(alternative))
            return std::nullopt;

        return AapPressAndHoldAirPodModeState{active, alternative};
    }

    bool AapPressAndHoldAirPodModeState::IsVolumeSupported() const
    {
        return (IsVolumeMode(_active[0]) && IsVolumeMode(_active[1])) ||
               (IsVolumeMode(_alternative[0]) && IsVolumeMode(_alternative[1]));
    }

    AapPressAndHoldAirPodModeState::ModePair AapPressAndHoldAirPodModeState::GetVolumeModes() const
    {
        ModePair volumes{
            AapPressAndHoldAirPodMode::VolumeDown,
            AapPressAndHoldAirPodMode::VolumeUp
        };

        for (std::size_t i = 0; i < volumes.size(); ++i)
        {
            if (IsVolumeMode(_active[i]))
                volumes[i] = _active[i];
            else if (IsVolumeMode(_alternative[i]))
                volumes[i] = _alternative[i];
        }

        return volumes;
    }

    AapPressAndHoldAirPodModeState::ModePair AapPressAndHoldAirPodModeState::GetStandardModes() const
    {
        ModePair modes{
            AapPressAndHoldAirPodMode::NoiseControl,
            AapPressAndHoldAirPodMode::NoiseControl
        };

        for (std::size_t i = 0; i < modes.size(); ++i)
        {
            if (IsStandardMode(_active[i]))
                modes[i] = _active[i];
            else if (IsStandardMode(_alternative[i]))
                modes[i] = _alternative[i];
        }

        return modes;
    }

    AapPressAndHoldAirPodModeState AapPressAndHoldAirPodModeState::SetRight(int value) const
    {
        if (value < 0 || value > 0xff)
            return *this;

        const auto right = static_cast<AapPressAndHoldAirPodMode>(value);
        auto result = *this;

        if (IsVolumeMode(right))
        {
            if (!IsVolumeSupported())
                return result;

            result._active = {
                right,
                right == AapPressAndHoldAirPodMode::VolumeUp
                    ? AapPressAndHoldAirPodMode::VolumeDown
                    : AapPressAndHoldAirPodMode::VolumeUp
            };
            result._alternative = GetStandardModes();
        }
        else if (IsStandardMode(right))
        {
            const auto modes = GetStandardModes();
            result._active = {right, modes[1]};
            result._alternative = IsVolumeSupported() ? GetVolumeModes() : result._active;
        }

        return result;
    }

    AapPressAndHoldAirPodModeState AapPressAndHoldAirPodModeState::SetLeft(int value) const
    {
        if (value < 0 || value > 0xff)
            return *this;

        const auto left = static_cast<AapPressAndHoldAirPodMode>(value);
        auto result = *this;

        if (IsVolumeMode(left))
        {
            if (!IsVolumeSupported())
                return result;

            result._active = {
                left == AapPressAndHoldAirPodMode::VolumeUp
                    ? AapPressAndHoldAirPodMode::VolumeDown
                    : AapPressAndHoldAirPodMode::VolumeUp,
                left
            };
            result._alternative = GetStandardModes();
        }
        else if (IsStandardMode(left))
        {
            const auto modes = GetStandardModes();
            result._active = {modes[0], left};
            result._alternative = IsVolumeSupported() ? GetVolumeModes() : result._active;
        }

        return result;
    }
}
