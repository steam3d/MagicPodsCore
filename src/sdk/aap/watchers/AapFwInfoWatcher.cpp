// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapFwInfoWatcher.h"

#include "sdk/aap/enums/AapCmd.h"

#include <algorithm>
#include <array>

namespace MagicPodsCore
{
    AapFwInfoWatcher::AapFwInfoWatcher() : AapWatcher{"AapFwInfoWatcher"}
    {
    }

    void AapFwInfoWatcher::ProcessResponse(const std::vector<unsigned char>& data)
    {
        if (data.size() <= ServiceFieldOffset)
            return;

        if (data[4] != static_cast<unsigned char>(AapCmd::FwInfo))
            return;

        // Bytes 7-8 payload length. The service field starts at byte 9
        // and is terminated by 0x00. Device information follows it as ordered strings.
        auto fieldStart = std::find(data.begin() + ServiceFieldOffset, data.end(), 0x00);
        if (fieldStart == data.end())
            return;

        ++fieldStart;

        std::array<std::string, FwInfoFieldCount> fields{};
        size_t fieldIndex = 0;
        while (fieldStart != data.end() && fieldIndex < fields.size())
        {
            const auto fieldEnd = std::find(fieldStart, data.end(), 0x00);
            fields[fieldIndex] = std::string(fieldStart, fieldEnd);
            ++fieldIndex;

            if (fieldEnd == data.end())
                break;

            fieldStart = fieldEnd + 1;
        }

        if (fieldIndex == 0 || fields[0].empty())
            return;

        AapFwInfo fwInfo{
            fields[0],
            fields[1],
            fields[2],
            fields[3],
            fields[4],
            fields[5],
            fields[6],
            fields[7],
            fields[8],
            fields[9],
            fields[10]
        };

        Logger::Debug("%s: %s", _tag.c_str(), fwInfo.ToString().c_str());
        _event.FireEvent(fwInfo);
    }
}
