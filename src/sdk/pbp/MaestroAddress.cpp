// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "MaestroAddress.h"

namespace MagicPodsCore
{
    uint32_t MaestroAddress::FromPeers(MaestroPeers source, MaestroPeers target)
    {
        return ((static_cast<uint32_t>(source) & 0x0F) << 6) | ((static_cast<uint32_t>(target) & 0x0F) << 10);
    }

    MaestroPeers MaestroAddress::GetSource(uint32_t address)
    {
        return static_cast<MaestroPeers>((address >> 6) & 0x0F);
    }

    MaestroPeers MaestroAddress::GetTarget(uint32_t address)
    {
        return static_cast<MaestroPeers>((address >> 10) & 0x0F);
    }

    std::optional<uint32_t> MaestroAddress::GetChannelId(MaestroPeers local, MaestroPeers remote)
    {
        if (local == MaestroPeers::MaestroA)
        {
            switch (remote)
            {
            case MaestroPeers::Case:
                return 18;
            case MaestroPeers::LeftBtCore:
                return 19;
            case MaestroPeers::LeftSensorHub:
                return 20;
            case MaestroPeers::RightBtCore:
                return 21;
            case MaestroPeers::RightSensorHub:
                return 22;
            default:
                return std::nullopt;
            }
        }
        else if (local == MaestroPeers::MaestroB)
        {
            switch (remote)
            {
            case MaestroPeers::Case:
                return 23;
            case MaestroPeers::LeftBtCore:
                return 24;
            case MaestroPeers::LeftSensorHub:
                return 25;
            case MaestroPeers::RightBtCore:
                return 26;
            case MaestroPeers::RightSensorHub:
                return 27;
            default:
                return std::nullopt;
            }
        }

        return std::nullopt;
    }

    std::optional<uint32_t> MaestroAddress::GetAddressForChannel(uint32_t channelId)
    {
        switch (channelId)
        {
        case 18:
            return FromPeers(MaestroPeers::MaestroA, MaestroPeers::Case);
        case 19:
            return FromPeers(MaestroPeers::MaestroA, MaestroPeers::LeftBtCore);
        case 20:
            return FromPeers(MaestroPeers::MaestroA, MaestroPeers::LeftSensorHub);
        case 21:
            return FromPeers(MaestroPeers::MaestroA, MaestroPeers::RightBtCore);
        case 22:
            return FromPeers(MaestroPeers::MaestroA, MaestroPeers::RightSensorHub);
        case 23:
            return FromPeers(MaestroPeers::MaestroB, MaestroPeers::Case);
        case 24:
            return FromPeers(MaestroPeers::MaestroB, MaestroPeers::LeftBtCore);
        case 25:
            return FromPeers(MaestroPeers::MaestroB, MaestroPeers::LeftSensorHub);
        case 26:
            return FromPeers(MaestroPeers::MaestroB, MaestroPeers::RightBtCore);
        case 27:
            return FromPeers(MaestroPeers::MaestroB, MaestroPeers::RightSensorHub);
        default:
            return std::nullopt;
        }
    }

    uint32_t MaestroAddress::GetProbeChannelId()
    {
        return GetChannelId(MaestroPeers::MaestroA, MaestroPeers::Case).value();
    }
}
