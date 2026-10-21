// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/structs/DeviceEarDetectionData.h"
#include "sdk/sgb/structs/GalaxyBudsEarDetectionStateArgs.h"

namespace MagicPodsCore
{
    class GalaxyBudsEarDetectionHelper
    {
    public:
        static DeviceEarDetectionData ToDeviceEarDetectionData(const GalaxyBudsEarDetectionStateArgs &state)
        {
            if (state.Left == GalaxyBudsEarDetectionState::Disconnected &&
                state.Right == GalaxyBudsEarDetectionState::Disconnected)
            {
                return DeviceEarDetectionData(DeviceEarStatus::NotAvailable);
            }

            // one active
            if (state.Left == GalaxyBudsEarDetectionState::Disconnected)
            {
                return DeviceEarDetectionData(
                    state.Right == GalaxyBudsEarDetectionState::Wearing ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
            }

            if (state.Right == GalaxyBudsEarDetectionState::Disconnected)
            {
                return DeviceEarDetectionData(
                    state.Left == GalaxyBudsEarDetectionState::Wearing ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
            }

            // both
            if (state.Left == GalaxyBudsEarDetectionState::Wearing &&
                state.Right == GalaxyBudsEarDetectionState::Wearing)
            {
                return DeviceEarDetectionData(DeviceEarStatus::Worn);
            }

            if (state.Left != GalaxyBudsEarDetectionState::Wearing &&
                state.Right != GalaxyBudsEarDetectionState::Wearing)
            {
                return DeviceEarDetectionData(DeviceEarStatus::NotWorn);
            }

            // one out
            return DeviceEarDetectionData(DeviceEarStatus::PartiallyWorn);
        }
    };
}
