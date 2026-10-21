// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/structs/DeviceEarDetectionData.h"
#include "sdk/aap/structs/AapEarDetectionArgs.h"

namespace MagicPodsCore
{
    class AapEarDetectionHelper
    {
    public:
        static DeviceEarDetectionData ToDeviceEarDetectionData(const AapEarDetectionArgs &state)
        {
            if (state.EarOne == AapEarDetectionState::_InClosedCase &&
                state.EarTwo == AapEarDetectionState::_InClosedCase)
            {
                return DeviceEarDetectionData(DeviceEarStatus::NotAvailable);
            }

            // one active
            if (state.EarOne == AapEarDetectionState::_InClosedCase)
            {
                return DeviceEarDetectionData(
                    state.EarTwo == AapEarDetectionState::InEar ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
            }

            if (state.EarTwo == AapEarDetectionState::_InClosedCase)
            {
                return DeviceEarDetectionData(
                    state.EarOne == AapEarDetectionState::InEar ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
            }

            // both
            if (state.EarOne == AapEarDetectionState::InEar && state.EarTwo == AapEarDetectionState::InEar)
            {
                return DeviceEarDetectionData(DeviceEarStatus::Worn);
            }

            if (state.EarOne != AapEarDetectionState::InEar && state.EarTwo != AapEarDetectionState::InEar)
            {
                return DeviceEarDetectionData(DeviceEarStatus::NotWorn);
            }

            // one out
            return DeviceEarDetectionData(DeviceEarStatus::PartiallyWorn);
        }
    };
}
