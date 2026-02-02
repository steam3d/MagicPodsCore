// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include <iostream>
#include "settings/SettingsService.h"
#include "settings/JsonToTomlConverter.h"

using namespace MagicPodsCore;

void printSeparator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n";
}

int main() {
    SettingsService settings(std::string(std::getenv("HOME")) + "/.config/magicpods/config.toml");

    printSeparator("1. Creating JSON with settings");

    nlohmann::json inputJson = {
        {"settings", {
            {"container1", {
                {"settings1", true},
                {"settings2", "two"},
                {"settings3", 3}
            }},
            {"container2", {
                {"settings2", "two"},
                {"settings3", 3}
            }},
            {"container3", {
                {"settings1", true}
            }}
        }}
    };

    std::cout << "Input JSON:\n" << inputJson.dump(2) << "\n";

    printSeparator("2. Converting JSON to TOML");

    toml::table tomlData = JsonToToml(inputJson);
    std::cout << "Converted TOML:\n" << tomlData << "\n";

    printSeparator("3. Saving settings via SaveSettings");

    settings.SaveSettings(tomlData);
    std::cout << "Settings saved successfully!\n";

    printSeparator("4. Getting settings back via GetSettingsAllWrapped");

    toml::table wrappedSettings = settings.GetSettingsAllWrapped();
    std::cout << "Wrapped TOML:\n" << wrappedSettings << "\n";

    printSeparator("5. Converting TOML back to JSON");

    nlohmann::json outputJson = TomlToJson(wrappedSettings);
    std::cout << "Output JSON:\n" << outputJson.dump(2) << "\n";

    printSeparator("6. Updating settings (merge test)");

    nlohmann::json updateJson = {
        {"settings", {
            {"container1", {
                {"settings1", false},
                {"settings4", "new value"}
            }},
            {"container4", {
                {"new_setting", 42}
            }}
        }}
    };

    std::cout << "Update JSON:\n" << updateJson.dump(2) << "\n";

    toml::table updateToml = JsonToToml(updateJson);
    settings.SaveSettings(updateToml);

    printSeparator("7. Final settings after merge");

    nlohmann::json finalJson = TomlToJson(settings.GetSettingsAllWrapped());
    std::cout << "Final JSON (merged):\n" << finalJson.dump(2) << "\n";

    printSeparator("Test completed successfully!");

    return 0;
}
