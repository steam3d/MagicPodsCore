#include "GalaxyBudsHelper.h"
#include <iostream>

namespace MagicPodsCore{

    GalaxyBudsModelIds GalaxyBudsHelper::GetModelFromColoredModel(GalaxyBudsColoredModelIds coloredModel)
    {
        if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds ||
            coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsUnknown)
        {
            return GalaxyBudsModelIds::GalaxyBuds;
        }
        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusBlue ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusPink ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusBlack ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusWhite ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusThomBrown ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusRed ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusDeepBlue ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusOlympic ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsPlusPurple)
        {
            return GalaxyBudsModelIds::GalaxyBudsPlus;
        }
        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsLiveBlack ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsLiveWhite ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsLiveBronze ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsLiveRed ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsLiveBlue ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsLiveThomBrown ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsLiveGrey)
        {
            return GalaxyBudsModelIds::GalaxyBudsLive;
        }
        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsProBlack ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsProSilver ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsProViolet ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsProWhite)
        {
            return GalaxyBudsModelIds::GalaxyBudsPro;

        }
        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2White ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2Black ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2Yellow ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2Green ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2Violet ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2ThomBrown ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2MaisonKitsune ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2AbsoluteBlack ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2Grey ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2Unknown)
        {
            return GalaxyBudsModelIds::GalaxyBuds2;
        }

        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2ProGrey ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2ProWhite ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds2ProViolet)
        {
            return GalaxyBudsModelIds::GalaxyBuds2Pro;
        }

        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsFeGraphite ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBudsFeWhite)
        {
            return GalaxyBudsModelIds::GalaxyBudsFe;
        }
        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds3Silver ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds3White)
        {
            return GalaxyBudsModelIds::GalaxyBuds3;
        }
        else if (coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds3ProSilver ||
                 coloredModel == GalaxyBudsColoredModelIds::GalaxyBuds3ProWhite)
        {
            return GalaxyBudsModelIds::GalaxyBuds3Pro;
        }
        else
        {
            return GalaxyBudsModelIds::Unknown;
        }
    }

    GalaxyBudsModelIds GalaxyBudsHelper::GetModelFromName(std::string name)
    {
        if (name.empty())
            return GalaxyBudsModelIds::Unknown;

        std::string _name = StringUtils::ToLowerCase(name);

        if (_name.find("galaxy buds (") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBuds;
        }
        else if (_name.find("galaxy buds+ (") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBudsPlus;
        }
        else if (_name.find("buds live") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBudsLive;
        }
        else if (_name.find("buds pro") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBudsPro;
        }
        else if (_name.find("buds2 pro") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBuds2Pro;
        }
        else if (_name.find("buds2") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBuds2;
        }
        else if (_name.find("buds fe") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBudsFe;
        }
        else if (_name.find("buds3 pro") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBuds3Pro;
        }
        else if (_name.find("buds3") != std::string::npos)
        {
            return GalaxyBudsModelIds::GalaxyBuds3;
        }
        else
        {
            return GalaxyBudsModelIds::Unknown;
        }
    }

    std::pair<GalaxyBudsModelIds, std::string> GalaxyBudsHelper::SearchModelColor(const std::vector<std::string>& guids, std::string name){
        std::vector<std::string> _guids;
        _guids.reserve(guids.size());
        std::transform(guids.begin(), guids.end(), std::back_inserter(_guids), [this](const std::string& guid) {
            return StringUtils::ToLowerCase(guid);
        });

        if (std::find(_guids.begin(), _guids.end(), GALAXYBUDSNEW) != _guids.end()) {

            auto it = std::find_if(_guids.begin(), _guids.end(), [](const std::string& guidStr) {
                return guidStr.find(deviceIdPrefix) == 0;
            });

            if (it != _guids.end()) {
                std::string foundGuid = *it;
                std::string hexId = foundGuid.substr(deviceIdPrefix.length()); //MUST BE LOWERCASE

                if(!hexId.empty() && hexId.length() == 4) {
                    int16_t id;
                    std::stringstream ss;
                    ss << std::hex << hexId;
                    ss >> id;

                    if (!ss.fail() && std::find(GalaxyBudsModelIdsArray.begin(), GalaxyBudsModelIdsArray.end(), id) != GalaxyBudsModelIdsArray.end())
                    {
                        GalaxyBudsColoredModelIds coloredModel = static_cast<GalaxyBudsColoredModelIds>(id);
                        GalaxyBudsModelIds model = this->GetModelFromColoredModel(coloredModel);
                        return { model, hexId };
                    }
                }
                else {
                    if (std::find(_guids.begin(), _guids.end(), HANDSFREESERVICECLASS) != _guids.end() &&
                        std::find(_guids.begin(), _guids.end(), LEAUDIO) != _guids.end()){
                            return { GalaxyBudsModelIds::GalaxyBuds2Pro, ""};
                    }
                    else {
                        return { GalaxyBudsModelIds::GalaxyBuds2, ""};
                    }
                }
            }
            };

        return { this->GetModelFromName(name), ""};
    }

    std::string GalaxyBudsHelper::GetServiceGuid(GalaxyBudsModelIds model){
        if (model == GalaxyBudsModelIds::GalaxyBuds){
            return std::string(GALAXYBUDSLEGASY);
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPlus) {
            return std::string(GALAXYBUDS);
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsLive) {
            return std::string(GALAXYBUDS);
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPro) {
            return std::string(GALAXYBUDS);
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds2) {
            return std::string(GALAXYBUDSNEW);
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds2Pro) {
            return std::string(GALAXYBUDSNEW);
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds3) {
            return std::string(GALAXYBUDSNEW);
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds3Pro) {
            return std::string(GALAXYBUDSNEW);
        }
        else {
            return "";
        }
    }
}
