// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapVolumeSwipeCapability.h"
#include "sdk/aap/enums/AapConversationAwarenessMode.h"
#include "sdk/aap/setters/AapSetVolumeSwipe.h"

namespace MagicPodsCore
{
    nlohmann::json AapVolumeSwipeCapability::CreateJsonBody()
    {    
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;        
        return bodyJson;
    }

    void AapVolumeSwipeCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapVolumeSwipeCapability::AapVolumeSwipeCapability(AapDevice& device) : AapCapability("volumeSwipe", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapVolumeSwipeMode mode){
            bool newOption = mode == AapVolumeSwipeMode::On;            
            
            if (!isAvailable){
                    //When we get ConversationAwareness mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = newOption;
                    Logger::Info("%s updated: %s", name.c_str(), AapVolumeSwipeModeToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
                else if (option != newOption){
                    option = newOption;
                    Logger::Info("%s updated: %s", name.c_str(), AapVolumeSwipeModeToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapVolumeSwipeCapability::~AapVolumeSwipeCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapVolumeSwipeCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_boolean())
        {
            bool selected = capability["selected"].get<bool>();
            AapVolumeSwipeMode nativeMode = selected ? AapVolumeSwipeMode::On : AapVolumeSwipeMode::Off;    
            SendData(AapSetVolumeSwipe(nativeMode));
            
            // Manually notify because AirPods will not send a notification that the option has been changed.
            if (option != selected){
                    option = selected;
                    Logger::Info("%s updated: %s", name.c_str(), AapVolumeSwipeModeToString(nativeMode).c_str());
                    _onChanged.FireEvent(*this);
            }

            Logger::Debug("%s::SetFromJson set option to %s", name.c_str(), AapVolumeSwipeModeToString(nativeMode).c_str());            
        }
        else
        {
            Logger::Error("Error: %s::SetFromJson got no value or value is not an boolean", name.c_str());
        }
    }
}
