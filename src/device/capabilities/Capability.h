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
        virtual nlohmann::json CreateJsonBody() = 0;

    public:
        explicit Capability(const std::string &name, bool isReadOnly) : name(name), isReadOnly(isReadOnly) {}
        Event<Capability> _onChanged{};
        nlohmann::json GetAsJson();
        virtual void SetFromJson(nlohmann::json json) = 0;

        virtual ~Capability() = default;
    };
}