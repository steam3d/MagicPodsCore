// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "StringUtils.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "sdk/sgb/enums/GalaxyBudsColoredModelIds.h"
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace MagicPodsCore
{
    class GalaxyBudsHelper
    {
    public:
        static bool IsGalaxyBudsDevice(const std::vector<std::string> &guids);
        static std::pair<GalaxyBudsModelIds, std::string> SearchModelColor(const std::vector<std::string> &guids, const std::string &name);
        static std::string GetServiceGuid(GalaxyBudsModelIds model);
        inline static const std::string GALAXYBUDSNEW = "2e73a4ad-332d-41fc-90e2-16bef06523f2";
        inline static const std::string GALAXYBUDS = "00001101-0000-1000-8000-00805f9b34fb";
        inline static const std::string GALAXYBUDSLEGASY = "00001102-0000-1000-8000-00805f9b34fd";
        inline static const std::string LEAUDIO = "0000184e-0000-1000-8000-00805f9b34fb";
        inline static const std::string HANDSFREESERVICECLASS = "0000111e-0000-1000-8000-00805f9b34fb";
        inline static const std::string deviceIdPrefix = "d908aab5-7a90-4cbe-8641-86a553db";

    private:
        static GalaxyBudsModelIds GetModelFromColoredModel(GalaxyBudsColoredModelIds coloredModel);
        static GalaxyBudsModelIds GetModelFromName(const std::string &name);
    };
}