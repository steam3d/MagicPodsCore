#include "Capability.h"

namespace MagicPodsCore
{
    nlohmann::json Capability::CreateJsonBody()
    {
        //Build crash if use abstract method
        return nlohmann::json();
    }

    nlohmann::json Capability::GetAsJson()
    {
        if (!isAvailable)
            return {};

        auto bodyJson = Capability::CreateJsonBody();
        bodyJson["IsReadOnly"] = isReadOnly;

        //auto json = nlohmann::json::object();
        //json[name] = bodyJson;

        return nlohmann::json{{name, bodyJson}};
    }
    void Capability::SetFromJson(nlohmann::json json)
    {
    }
}