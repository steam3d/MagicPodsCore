#include "Capability.h"

namespace MagicPodsCore
{
    nlohmann::json Capability::CreateJsonBody()
    {
        return nlohmann::json();
    }

    void Capability::Reset()
    {
        isAvailable = false;
        LOG_DEBUG("Capability: %s was reset", name.c_str());
    }

    nlohmann::json Capability::GetAsJson()
    {
        if (!isAvailable)
            return {};

        auto bodyJson = CreateJsonBody();
        bodyJson["IsReadOnly"] = isReadOnly;

        return nlohmann::json{{name, bodyJson}};
    }
    void Capability::SetFromJson(const nlohmann::json &json)
    {
    }
}