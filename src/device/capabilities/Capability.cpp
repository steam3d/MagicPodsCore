#include "Capability.h"

namespace MagicPodsCore
{
    nlohmann::json Capability::CreateJsonBody()
    {
        LOG_DEBUG("Capability CreateJsonBody");
        //Build crash if use abstract method
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

        //auto json = nlohmann::json::object();
        //json[name] = bodyJson;

        return nlohmann::json{{name, bodyJson}};
    }
    void Capability::SetFromJson(nlohmann::json json)
    {
    }
}