// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapVolumeSwipeLengthCapability.h"
#include "sdk/aap/setters/AapSetVolumeSwipeLength.h"

namespace MagicPodsCore
{
    nlohmann::json AapVolumeSwipeLengthCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapVolumeSwipeLengthCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapVolumeSwipeLengthCapability::AapVolumeSwipeLengthCapability(AapDevice& device) : AapCapability("volumeSwipeLength", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapVolumeSwipeLengthMode mode){
            if (!isAvailable){
                    //When we get ConversationAwareness mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = mode;
                    Logger::Info("%s updated: %s",name.c_str(), AapVolumeSwipeLengthModeToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
                else if (option != mode){
                    option = mode;
                    Logger::Info("%s updated: %s",name.c_str(), AapVolumeSwipeLengthModeToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapVolumeSwipeLengthCapability::~AapVolumeSwipeLengthCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapVolumeSwipeLengthCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_number_integer())
        {
            unsigned char selected = static_cast<unsigned char>(capability["selected"].get<int>());
            if (isValidAapVolumeSwipeLengthMode(selected)){
                AapVolumeSwipeLengthMode nativeMode = static_cast<AapVolumeSwipeLengthMode>(selected);
                SendData(AapSetVolumeSwipeLength(nativeMode));

                // Manually notify because AirPods will not send a notification that the option has been changed.
                if (option != nativeMode){
                    option = nativeMode;
                    Logger::Info("%s updated: %s", name.c_str(), AapVolumeSwipeLengthModeToString(nativeMode).c_str());
                    _onChanged.FireEvent(*this);
                }

                Logger::Debug("%s::SetFromJson set option to %s", name.c_str(), AapVolumeSwipeLengthModeToString(nativeMode).c_str());
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
