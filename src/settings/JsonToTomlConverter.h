// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <nlohmann/json.hpp>
#include <toml++/toml.h>

namespace MagicPodsCore {

toml::table JsonToToml(const nlohmann::json& json);
toml::array JsonArrayToToml(const nlohmann::json& jsonArray);
toml::table JsonObjectToToml(const nlohmann::json& jsonObject);

nlohmann::json TomlToJson(const toml::table& toml);
nlohmann::json TomlNodeViewToJson(const toml::node_view<toml::node>& nodeView);

}
