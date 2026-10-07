// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "device/enums/DeviceEarStatus.h"
#include <sstream>

namespace MagicPodsCore
{
    enum class DeviceEarMediaStatus : unsigned char
    {
        NotAvailable = 0x00,
        Play = 0x01,
        Pause = 0x02,
    };

    struct DeviceEarDetectionData
    {
        DeviceEarStatus Status{DeviceEarStatus::NotAvailable};

        DeviceEarDetectionData() = default;
        DeviceEarDetectionData(DeviceEarStatus status){
            Status = status;
        }

        std::string ToString() const
        {
            std::ostringstream os;
            os << "EarDetection: " << DeviceEarStatusToString(Status).c_str();

            return os.str();
        }

        DeviceEarMediaStatus GetMediaStatus()
        {
            switch (Status){
                case DeviceEarStatus::NotAvailable:
                    return DeviceEarMediaStatus::NotAvailable;
                case DeviceEarStatus::Worn:
                    return DeviceEarMediaStatus::Play;
                case DeviceEarStatus::PartiallyWorn:
                    return DeviceEarMediaStatus::Pause;
                case DeviceEarStatus::NotWorn:
                    return DeviceEarMediaStatus::Pause;
                default:
                    return DeviceEarMediaStatus::NotAvailable;
            }
        }

        bool AreStatesEqual(DeviceEarDetectionData state)
        {
            return Status == state.Status;
        }
    };
}
