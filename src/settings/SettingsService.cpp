// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "SettingsService.h"
#include <stdexcept>
#include <fstream>
#include <filesystem>

namespace MagicPodsCore {

SettingsService::SettingsService(const std::string& filePath) : _filePath(filePath), _onSettingUpdate{} {
    LoadFromFile();
}

void SettingsService::LoadFromFile() {
    std::ifstream file(_filePath);
    if (!file.is_open()) {
        return;
    }

    try {
        _settings = toml::parse_file(_filePath);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error("Failed to parse TOML file: " + std::string(e.what()));
    }
}

void SettingsService::WriteToFile() {
    std::filesystem::create_directories(std::filesystem::path(_filePath).parent_path());

    std::ofstream file(_filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + _filePath);
    }

    file << _settings;

    if (!file.good()) {
        throw std::runtime_error("Failed to write to file: " + _filePath);
    }
}

const toml::table& SettingsService::GetSettingsAll() {
    return _settings;
}

toml::table SettingsService::GetSettingsAllWrapped() {
    toml::table wrapped;
    wrapped.insert_or_assign("settings", _settings);
    return wrapped;
}

const toml::table& SettingsService::GetSettings(const std::string& container) {
    auto containerTable = _settings[container].as_table();
    if (!containerTable) {
        _settings.insert_or_assign(container, toml::table{});
        containerTable = _settings[container].as_table();
    }
    return *containerTable;
}

toml::node_view<toml::node> SettingsService::GetSetting(const std::string& container, const std::string& name) {
    auto containerTable = _settings[container].as_table();
    if (!containerTable) {
        return toml::node_view<toml::node>{};
    }
    return (*containerTable)[name];
}

void SettingsService::SaveSettings(const toml::table& table) {
    auto settingsRoot = table["settings"].as_table();
    if (!settingsRoot) {
        throw std::invalid_argument("Table must contain 'settings' root key");
    }

    for (const auto& [containerName, containerData] : *settingsRoot) {
        if (auto incomingContainer = containerData.as_table()) {
            auto existingContainer = _settings[containerName].as_table();
            if (!existingContainer) {
                _settings.insert_or_assign(containerName, toml::table{});
                existingContainer = _settings[containerName].as_table();
            }

            for (const auto& [key, value] : *incomingContainer) {
                existingContainer->insert_or_assign(key, value);
            }
        }
    }

    WriteToFile();

    for (const auto& [containerName, containerData] : *settingsRoot) {
        if (auto incomingContainer = containerData.as_table()) {
            auto existingContainer = _settings[containerName].as_table();
            for (const auto& [key, value] : *incomingContainer) {
                auto savedValue = (*existingContainer)[key];
                _onSettingUpdate.FireEvent(UpdatedSettingNotification{containerName, key, savedValue});
            }
        }
    }
}

void SettingsService::SaveSetting(const std::string& container, const std::string& name, const toml::node& value) {
    auto containerTable = _settings[container].as_table();
    if (!containerTable) {
        _settings.insert_or_assign(container, toml::table{});
        containerTable = _settings[container].as_table();
    }
    containerTable->insert_or_assign(name, value);
    WriteToFile();

    auto savedValue = (*containerTable)[name];
    _onSettingUpdate.FireEvent(UpdatedSettingNotification{container, name, savedValue});
}

void SettingsService::SaveSetting(const std::string& container, const std::string& name, const toml::node_view<const toml::node>& value) {
    if (value) {
        auto containerTable = _settings[container].as_table();
        if (!containerTable) {
            _settings.insert_or_assign(container, toml::table{});
            containerTable = _settings[container].as_table();
        }
        containerTable->insert_or_assign(name, *value.node());
        WriteToFile();

        auto savedValue = (*containerTable)[name];
        _onSettingUpdate.FireEvent(UpdatedSettingNotification{container, name, savedValue});
    }
}

std::string SettingsService::GetConfigPath(const std::string &fileName) {
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"))
        return std::string(xdg) + "/magicpods/" + fileName;

    if (const char* home = std::getenv("HOME"))
        return std::string(home) + "/.config/magicpods/" + fileName;

    throw std::runtime_error("Cannot determine config path");    
}
}
