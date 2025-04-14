// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>
#include <json.hpp>
#include "Event.h"

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
        virtual void Reset();

    public:
        explicit Capability(const std::string &name, bool isReadOnly) : name(name), isReadOnly(isReadOnly) {}
        Event<Capability> &GetChangedEvent()
        {
            return _onChanged;
        }

        const std::string& GetName() const {
            return name;
        }

        nlohmann::json GetAsJson();
        virtual void SetFromJson(const nlohmann::json &json);
        virtual ~Capability() = default;
    };
}