// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore
{
    // Byte 6 indicates the setting type
    enum class AapCmdSettings : unsigned char
    {
        //Events
        Anc = 0x0d,
        ConversationAwareness = 0x28,
        NoiseCancellationOneAirPod = 0x1b,
        PressAndHoldAirPodMode = 0x16, // Can't get current state from AirPods
        PressAndHoldDuration = 0x18,
        PressAndHoldAirPodModeAnc = 0x1a, // Can't get current state from AirPods
        DigitalCrown = 0x1c, // Can't get current state from AirPods
        PressSpeed = 0x17,
        VolumeSwipe = 0x25,
        VolumeSwipeLength = 0x23,
        PersonalizedVolume = 0x26,
        ToneVolume = 0x1f,
        EndCall = 0x24,
        AdaptiveAudioNoise = 0x2e,
        AncOffMode = 0x34, // Can't get current state from AirPods
        StemControl = 0x39, //Can't get current state from AirPods
    };
}
