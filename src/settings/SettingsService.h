// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <map>
#include <optional>
#include <string>
#include <type_traits>
#include <toml++/toml.h>
#include "Event.h"
#include "UpdatedSettingNotification.h"

namespace MagicPodsCore {

class SettingsService {
private:
    toml::table _settings{};
    std::string _filePath;
    Event<UpdatedSettingNotification> _onSettingUpdate{};

    void LoadFromFile();
    void WriteToFile();

public:
    SettingsService(const std::string& filePath);
    const toml::table& GetSettingsAll();
    toml::table GetSettingsAllWrapped();
    const toml::table& GetSettings(const std::string& container);
    toml::node_view<toml::node> GetSetting(const std::string& container, const std::string& name);

    void SaveSettings(const toml::table& table);

    template<typename T>
    void SaveSetting(const std::string& container, const std::string& name, T&& value);

    void SaveSetting(const std::string& container, const std::string& name, const toml::node& value);
    void SaveSetting(const std::string& container, const std::string& name, const toml::node_view<const toml::node>& value);

    Event<UpdatedSettingNotification>& GetOnSettingUpdateEvent() {
        return _onSettingUpdate;
    }
};

template<typename T>
void SettingsService::SaveSetting(const std::string& container, const std::string& name, T&& value) {
    auto containerTable = _settings[container].as_table();
    if (!containerTable) {
        _settings.insert_or_assign(container, toml::table{});
        containerTable = _settings[container].as_table();
    }
    containerTable->insert_or_assign(name, std::forward<T>(value));
    WriteToFile();

    auto savedValue = (*containerTable)[name];
    _onSettingUpdate.FireEvent(UpdatedSettingNotification{container, name, savedValue});
}

}
