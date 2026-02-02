// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>
#include <toml++/toml.h>

namespace MagicPodsCore {

struct UpdatedSettingNotification {
private:
    std::string_view _containerName;
    std::string_view _settingName;
    toml::node_view<toml::node> _value;

public:
    UpdatedSettingNotification(std::string_view containerName, std::string_view settingName, toml::node_view<toml::node> value) 
    : _containerName(containerName), _settingName(settingName), _value(value) {}

    std::string_view GetContainerName() const {
        return _containerName;
    }

    std::string_view GetSettingName() const {
        return _settingName;
    }

    toml::node_view<toml::node> GetValue() const {
        return _value;
    }
};

}
