// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapNoiseCancellationOneAirPodModeCapability.h"
#include "sdk/aap/enums/AapNoiseCancellationOneAirPodMode.h"
#include "sdk/aap/setters/AapSetNoiseCancellationOneAirPod.h"

namespace MagicPodsCore
{
    nlohmann::json AapNoiseCancellationOneAirPodModeCapability::CreateJsonBody()
    {    
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;        
        return bodyJson;
    }

    void AapNoiseCancellationOneAirPodModeCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapNoiseCancellationOneAirPodModeCapability::AapNoiseCancellationOneAirPodModeCapability(AapDevice& device) : AapCapability("ancOneAirPod", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapNoiseCancellationOneAirPodMode mode){
            bool newOption = mode == AapNoiseCancellationOneAirPodMode::On;            
            
            if (!isAvailable){
                    //When we get ConversationAwareness mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = newOption;
                    Logger::Info("%s updated: %s",name.c_str(), AapNoiseCancellationOneAirPodModeToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
                else if (option != newOption){
                    option = newOption;
                    Logger::Info("%s updated: %s",name.c_str(), AapNoiseCancellationOneAirPodModeToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapNoiseCancellationOneAirPodModeCapability::~AapNoiseCancellationOneAirPodModeCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapNoiseCancellationOneAirPodModeCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_boolean())
        {
            bool selected = capability["selected"].get<bool>();
            AapNoiseCancellationOneAirPodMode nativeMode = selected ? AapNoiseCancellationOneAirPodMode::On : AapNoiseCancellationOneAirPodMode::Off;    
            SendData(AapSetNoiseCancellationOneAirPod(nativeMode));
            
            // Manually notify because AirPods will not send a notification that the option has been changed.
            if (option != selected){
                    option = selected;
                    Logger::Info("%s updated: %s", name.c_str(), AapNoiseCancellationOneAirPodModeToString(nativeMode).c_str());
                    _onChanged.FireEvent(*this);
            }

            Logger::Debug("%s::SetFromJson set option to %s", name.c_str(), AapNoiseCancellationOneAirPodModeToString(nativeMode).c_str());            
        }
        else
        {
            Logger::Error("Error: %s::SetFromJson got no value or value is not an boolean", name.c_str());
        }
    }
}
