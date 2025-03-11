#pragma once

#include "GalaxyBudsModelIds.h"
#include "GalaxyBudsColoredModelIds.h"
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace MagicPodsCore
{
    class GalaxyBudsHelper{
        public:
            std::pair<GalaxyBudsModelIds, std::string> SearchModelColor(const std::vector<std::string>& guids, std::string name);
            inline static const std::string SONYCONTROL1          = "96cc203e-5068-46ad-b32d-e316f5e069ba";
            inline static const std::string SONYCONTROL2          = "ba69e0f5-16e3-2db3-ad46-68503e20cc96";
            inline static const std::string GALAXYBUDSNEW         = "2e73a4ad-332d-41fc-90e2-16bef06523f2";
            inline static const std::string GALAXYBUDS            = "00001101-0000-1000-8000-00805f9b34fb";
            inline static const std::string GALAXYBUDSLEGASY      = "00001102-0000-1000-8000-00805f9b34fd";
            inline static const std::string LEAUDIO               = "0000184e-0000-1000-8000-00805f9b34fb";
            inline static const std::string HANDSFREESERVICECLASS = "0000111e-0000-1000-8000-00805f9b34fb";
            inline static const std::string deviceIdPrefix        = "d908aab5-7a90-4cbe-8641-86a553db";
            std::string toLowerCase(const std::string& str);
        private:
            GalaxyBudsModelIds GetModelFromColoredModel(GalaxyBudsColoredModelIds coloredModel);
            GalaxyBudsModelIds GetModelFromName(std::string name);
            std::string GetServiceGuid(GalaxyBudsModelIds model);
    };
}