// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// "is a game running" signal

#include "SteamGamePresence.h"

#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <string>

namespace MagicPodsCore
{
    namespace
    {
        // "reaper SteamLaunch AppId=1091500" - the argument Steam uses for every game process,
        // including Proton titles, so this does not care which runtime the game uses.
        constexpr const char* REAPER_MARKER = "AppId=";

        std::mutex g_cacheMutex{};
        std::optional<int> g_cachedAppId{};
        std::chrono::steady_clock::time_point g_cacheTime{};

        bool ReadCmdline(const std::string& path, std::string& out)
        {
            FILE* file = std::fopen(path.c_str(), "rb");
            if (!file)
            {
                return false;
            }
            char buffer[4096];
            const size_t read = std::fread(buffer, 1, sizeof(buffer), file);
            std::fclose(file);
            // cmdline separates arguments with NUL; keep them, the marker search does not care.
            out.assign(buffer, read);
            return read > 0;
        }
    }

    std::optional<int> SteamGamePresence::RunningAppId()
    {
        std::error_code ec{};
        std::filesystem::directory_iterator it{"/proc", ec};
        if (ec)
        {
            return std::nullopt;
        }
        for (const auto& entry : it)
        {
            const std::string name = entry.path().filename().string();
            if (name.empty() || name[0] < '0' || name[0] > '9')
            {
                continue;
            }
            std::string cmdline{};
            if (!ReadCmdline(entry.path() / "cmdline", cmdline) || cmdline.find(REAPER_MARKER) == std::string::npos)
            {
                continue;
            }
            const size_t at = cmdline.find(REAPER_MARKER) + std::strlen(REAPER_MARKER);
            int appId = 0;
            bool any = false;
            for (size_t i = at; i < cmdline.size() && std::isdigit(static_cast<unsigned char>(cmdline[i])); ++i)
            {
                appId = appId * 10 + (cmdline[i] - '0');
                any = true;
            }
            if (any)
            {
                return appId;
            }
        }
        return std::nullopt;
    }

    bool SteamGamePresence::IsGameRunning(int ttlSeconds)
    {
        std::lock_guard lock{g_cacheMutex};
        const auto now = std::chrono::steady_clock::now();
        if (ttlSeconds > 0 && g_cacheTime.time_since_epoch().count() != 0 &&
            now - g_cacheTime < std::chrono::seconds{ttlSeconds})
        {
            return g_cachedAppId.has_value();
        }
        g_cachedAppId = RunningAppId();
        g_cacheTime = now;
        return g_cachedAppId.has_value();
    }
}
