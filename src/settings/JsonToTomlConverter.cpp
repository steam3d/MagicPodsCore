// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "JsonToTomlConverter.h"
#include <stdexcept>

namespace MagicPodsCore {

toml::array JsonArrayToToml(const nlohmann::json& jsonArray) {
    toml::array tomlArray;
    for (const auto& item : jsonArray) {
        if (item.is_null()) {
            continue;
        } else if (item.is_boolean()) {
            tomlArray.push_back(item.get<bool>());
        } else if (item.is_number_integer()) {
            tomlArray.push_back(item.get<int64_t>());
        } else if (item.is_number_float()) {
            tomlArray.push_back(item.get<double>());
        } else if (item.is_string()) {
            tomlArray.push_back(item.get<std::string>());
        } else if (item.is_array()) {
            tomlArray.push_back(JsonArrayToToml(item));
        } else if (item.is_object()) {
            tomlArray.push_back(JsonObjectToToml(item));
        }
    }
    return tomlArray;
}

toml::table JsonObjectToToml(const nlohmann::json& jsonObject) {
    toml::table tomlTable;
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        const auto& key = it.key();
        const auto& value = it.value();

        if (value.is_null()) {
            continue;
        } else if (value.is_boolean()) {
            tomlTable.insert_or_assign(key, value.get<bool>());
        } else if (value.is_number_integer()) {
            tomlTable.insert_or_assign(key, value.get<int64_t>());
        } else if (value.is_number_float()) {
            tomlTable.insert_or_assign(key, value.get<double>());
        } else if (value.is_string()) {
            tomlTable.insert_or_assign(key, value.get<std::string>());
        } else if (value.is_array()) {
            tomlTable.insert_or_assign(key, JsonArrayToToml(value));
        } else if (value.is_object()) {
            tomlTable.insert_or_assign(key, JsonObjectToToml(value));
        }
    }
    return tomlTable;
}

// Supported types: bool, int64_t, double, string, array, object (recursive)
// null values are skipped
toml::table JsonToToml(const nlohmann::json& json) {
    if (!json.is_object()) {
        throw std::invalid_argument("JSON root must be an object");
    }
    return JsonObjectToToml(json);
}

static nlohmann::json TomlNodeToJson(const toml::node& node);

static nlohmann::json TomlArrayToJson(const toml::array& tomlArray) {
    nlohmann::json jsonArray = nlohmann::json::array();
    for (const auto& item : tomlArray) {
        jsonArray.push_back(TomlNodeToJson(item));
    }
    return jsonArray;
}

static nlohmann::json TomlTableToJson(const toml::table& tomlTable) {
    nlohmann::json jsonObject = nlohmann::json::object();
    for (const auto& [key, value] : tomlTable) {
        jsonObject[key] = TomlNodeToJson(value);
    }
    return jsonObject;
}

static nlohmann::json TomlNodeToJson(const toml::node& node) {
    if (auto val = node.as_boolean()) {
        return val->get();
    } else if (auto val = node.as_integer()) {
        return val->get();
    } else if (auto val = node.as_floating_point()) {
        return val->get();
    } else if (auto val = node.as_string()) {
        return val->get();
    } else if (auto val = node.as_array()) {
        return TomlArrayToJson(*val);
    } else if (auto val = node.as_table()) {
        return TomlTableToJson(*val);
    }
    return nullptr;
}

// Supported types: boolean, integer, floating_point, string, array, table (recursive)
// Unknown types are converted to null
nlohmann::json TomlToJson(const toml::table& toml) {
    return TomlTableToJson(toml);
}

nlohmann::json TomlNodeViewToJson(const toml::node_view<toml::node>& nodeView) {
    if (!nodeView) {
        return nullptr;
    }
    return TomlNodeToJson(*nodeView.node());
}

}
