// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapAdaptiveAudioNoiseCapability.h"
#include "sdk/aap/setters/AapSetAdaptiveAudioNoise.h"
#include <cmath>

namespace MagicPodsCore
{
    nlohmann::json AapAdaptiveAudioNoiseCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapAdaptiveAudioNoiseCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapAdaptiveAudioNoiseCapability::AapAdaptiveAudioNoiseCapability(AapDevice& device) : AapCapability("adaptiveAudioNoise", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, unsigned char mode){            
            if (!isAvailable){
                    //When we get ConversationAwareness mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = mode;
                    Logger::Info("%s updated: %d",name.c_str(), mode);
                    _onChanged.FireEvent(*this);
                }
                else if (option != mode){
                    option = mode;
                    Logger::Info("%s updated: %d",name.c_str(), mode);
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapAdaptiveAudioNoiseCapability::~AapAdaptiveAudioNoiseCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapAdaptiveAudioNoiseCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_number_integer())
        {
            unsigned char selected = static_cast<unsigned char>(capability["selected"].get<int>());            
            if (selected >= 0 && selected <= 100){
                unsigned char nativeMode = static_cast<unsigned char>(selected);                
                SendData(AapSetAdaptiveAudioNoise(nativeMode));

                // Manually notify because AirPods will not send a notification that the option has been changed.
                if (option != selected){
                    option = selected;
                    Logger::Info("%s updated: %d",name.c_str(), nativeMode);
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
