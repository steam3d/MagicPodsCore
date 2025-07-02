// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapToneVolumeCapability.h"
#include "sdk/aap/setters/AapSetToneVolume.h"
#include <cmath>

namespace MagicPodsCore
{
    nlohmann::json AapToneVolumeCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    unsigned char AapToneVolumeCapability::ConvertToRange15_100(unsigned char value){
        float uiVol = static_cast<float>(value);

        float uiMin = 15.0f;
        float uiMax = 125.0f;

        float aapMin = 15.0f;
        float aapMax = 100.0f;

        float aapVol = ((uiVol - uiMin) / (uiMax - uiMin) * (aapMax - aapMin)) + aapMin;

        if (aapVol < aapMin)
            aapVol = aapMin;
            
        if (aapVol > aapMax)
            aapVol = aapMax;
        
        return static_cast<unsigned char>(std::round(aapVol));
    }

    unsigned char AapToneVolumeCapability::ConvertToRange15_125(unsigned char value){
        float aapVol = static_cast<float>(value);

        float uiMin = 15.0f;
        float uiMax = 125.0f;

        float aapMin = 15.0f;
        float aapMax = 100.0f;
        
        float uiVol = ((aapVol - aapMin) / (aapMax - aapMin)) * (uiMax - uiMin) + uiMin;

        if (uiVol < uiMin)
            uiVol = uiMin;
            
        if (uiVol > uiMax)
            uiVol = uiMax;

        return static_cast<unsigned char>(std::round(uiVol));
    }

    void AapToneVolumeCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapToneVolumeCapability::AapToneVolumeCapability(AapDevice& device) : AapCapability("toneVolume", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, unsigned char mode){
            unsigned char newOption = ConvertToRange15_125(mode);
            if (!isAvailable){
                    //When we get ConversationAwareness mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = newOption;
                    Logger::Info("%s updated: r:%d ui:%d",name.c_str(), mode, newOption);
                    _onChanged.FireEvent(*this);
                }
                else if (option != newOption){
                    option = newOption;
                    Logger::Info("%s updated: r:%d ui:%d",name.c_str(), mode, newOption);
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapToneVolumeCapability::~AapToneVolumeCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapToneVolumeCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_number_integer())
        {
            unsigned char selected = static_cast<unsigned char>(capability["selected"].get<int>());            
            if (selected >= 15 && selected <= 125){
                unsigned char nativeMode = ConvertToRange15_100(selected);
                SendData(AapSetToneVolume(nativeMode));

                // Manually notify because AirPods will not send a notification that the option has been changed.
                if (option != selected){
                    option = selected;
                    Logger::Info("%s updated: r:%d ui:%d",name.c_str(), nativeMode, selected);
                    _onChanged.FireEvent(*this);
                }

                Logger::Debug("%s::SetFromJson set option to %d", name.c_str(), nativeMode);
            }
            else{
                Logger::Error("Error: %s::SetFromJson got unexpected option: %d", name.c_str(), selected);
            }
        }
        else
        {
            Logger::Error("Error: %s::SetFromJson got no value or value is not an integer", name.c_str());
        }
    }
}
