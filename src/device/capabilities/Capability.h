#pragma once

#include <string>
#include <json.hpp>
#include "../../Event.h"
#include "../../Logger.h"

namespace MagicPodsCore
{
    class Capability
    {
    protected:
        std::string name{};
        bool isReadOnly = false;
        bool isAvailable = false;
        Event<Capability> _onChanged{};
        virtual nlohmann::json CreateJsonBody();

    public:
        explicit Capability(const std::string &name, bool isReadOnly) : name(name), isReadOnly(isReadOnly) {}
        Event<Capability> &GetChangedEvent()
        {
            return _onChanged;
        }
        nlohmann::json GetAsJson();
        virtual void SetFromJson(nlohmann::json json);        
        virtual ~Capability() = default;
    };
}