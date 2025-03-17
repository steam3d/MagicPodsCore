#include "Capability.h"

namespace MagicPodsCore
{
    nlohmann::json Capability::GetAsJson()
    {
        if (!isAvailable)
            return nlohmann::json{};

        auto bodyJson = Capability::CreateJsonBody();
        bodyJson["IsReadOnly"] = isReadOnly;

        auto json = nlohmann::json::object();
        json[name] = bodyJson;

        return json;
    }
}