// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "CmnSendHexCapability.h"

#include "Logger.h"
#include "StringUtils.h"
#include "device/Device.h"

#include <algorithm>
#include <cctype>

namespace MagicPodsCore
{
    bool CmnSendHexCapability::IsValidHex(const std::string& value)
    {
        return !value.empty() &&
               value.size() % 2 == 0 &&
               std::all_of(value.begin(), value.end(), [](unsigned char character)
               {
                   return std::isxdigit(character) != 0;
               });
    }

    CmnSendHexCapability::CmnSendHexCapability(Device& device)
        : Capability("sendHex", false), _device{device}{}

    void CmnSendHexCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_string())
        {
            Logger::Error("Error: CmnSendHexCapability::SetFromJson got no value or value is not a string");
            return;
        }

        const auto selected = capability["selected"].get<std::string>();
        if (!IsValidHex(selected))
        {
            Logger::Error("Error: CmnSendHexCapability::SetFromJson got invalid hex data");
            return;
        }

        _device.SendRawData(StringUtils::HexStringToBytes(selected));
    }
}
