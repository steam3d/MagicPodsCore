// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapRawConversationAwarenessStateCapability.h"
#include "sdk/aap/enums/AapConversationAwarenessState.h"

namespace MagicPodsCore
{
    bool AapRawConversationAwarenessStateCapability::AapConversationAwarenessStateToBoolean(AapConversationAwarenessState mode)
    {
        if (mode == AapConversationAwarenessState::StartSpeaking){
            return true;
        }
        return false;
    }

    nlohmann::json AapRawConversationAwarenessStateCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapRawConversationAwarenessStateCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapRawConversationAwarenessStateCapability::AapRawConversationAwarenessStateCapability(AapDevice& device) : AapCapability("rawConversationAwarenessSpeaking", true, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapConversationAwarenessState mode){
            bool newOption = AapConversationAwarenessStateToBoolean(mode);

            if (!isAvailable){
                    //When we get ConversationAwareness mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = newOption;
                    Logger::Info("RawConversationAwarenessState updated: %s", AapConversationAwarenessStateToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
                else if (option != newOption){
                    option = newOption;
                    Logger::Info("RawConversationAwarenessState updated: %s", AapConversationAwarenessStateToString(mode).c_str());
                    _onChanged.FireEvent(*this);
                }
        });
    }

    AapRawConversationAwarenessStateCapability::~AapRawConversationAwarenessStateCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }
}
