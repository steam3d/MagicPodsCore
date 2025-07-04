// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapMuteMicrophoneEndCallCapability.h"
#include "sdk/aap/setters/AapSetMuteMicrophoneEndCall.h"

namespace MagicPodsCore
{
    nlohmann::json AapMuteMicrophoneEndCallCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapMuteMicrophoneEndCallCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapMuteMicrophoneEndCallCapability::AapMuteMicrophoneEndCallCapability(AapDevice& device) : AapCapability("endCall", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapMuteMicrophoneEndCallMode mode){            
            if (!isAvailable){
                    //When we get ConversationAwareness mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = mode.endCallMode;
                    Logger::Info("%s updated: %s",name.c_str(), AapEndCallModeToString(mode.endCallMode).c_str());
                    _onChanged.FireEvent(*this);
                }
                else if (option != mode.endCallMode){
                    option = mode.endCallMode;
                    Logger::Info("%s updated: %s",name.c_str(), AapEndCallModeToString(mode.endCallMode).c_str());
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapMuteMicrophoneEndCallCapability::~AapMuteMicrophoneEndCallCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapMuteMicrophoneEndCallCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_number_integer())
        {
            unsigned char selected = static_cast<unsigned char>(capability["selected"].get<int>());
            if (isValidAapEndCallMode(selected)){
                AapEndCallMode nativeMode = static_cast<AapEndCallMode>(selected);
                SendData(AapSetMuteMicrophoneEndCall(nativeMode));

                //AirPods will send notification after change
                // Manually notify because AirPods will not send a notification that the option has been changed.
                //if (option != nativeMode){
                //    option = nativeMode;
                //    Logger::Info("%s updated: %s", name.c_str(), AapEndCallModeToString(nativeMode).c_str());
                //    _onChanged.FireEvent(*this);
                //}

                Logger::Debug("%s::SetFromJson set option to %s", name.c_str(), AapEndCallModeToString(nativeMode).c_str());
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
