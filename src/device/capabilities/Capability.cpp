// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "Capability.h"
#include "Logger.h"

namespace MagicPodsCore
{
    nlohmann::json Capability::CreateJsonBody()
    {
        return nlohmann::json::object();
    }

    void Capability::Reset()
    {
        isAvailable = false;
        Logger::Debug("Capability: %s was reset", name.c_str());
    }

    nlohmann::json Capability::GetAsJson()
    {
        if (!isAvailable)
            return {};

        auto bodyJson = CreateJsonBody();
        bodyJson["readonly"] = isReadOnly;

        return nlohmann::json{{name, bodyJson}};
    }
    void Capability::SetFromJson(const nlohmann::json &json)
    {
    }
}