// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapConversationAwarenessCapability.h"
#include "sdk/aap/enums/AapConversationAwarenessMode.h"
#include "sdk/aap/setters/AapSetConversationAwarenessStatus.h"

namespace MagicPodsCore
{
    int AapConversationAwarenessCapability::LoadVolumeSetting()
    {
        std::optional<int64_t> setting = device.LoadSettingInt("conversation_awareness_volume");
        if (!setting.has_value() || setting.value() <= 0)
            return 0;
        if (setting.value() > 100)
            return 100;

        return static_cast<int>(setting.value());
    }

    void AapConversationAwarenessCapability::SaveVolumeSetting(int value)
    {
        device.SaveSettingInt("conversation_awareness_volume", value);
    }

    nlohmann::json AapConversationAwarenessCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = nlohmann::json::object();
        bodyJson["selected"]["enabled"] = option;
        bodyJson["selected"]["volume"] = LoadVolumeSetting();
        return bodyJson;
    }

    void AapConversationAwarenessCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapConversationAwarenessCapability::AapConversationAwarenessCapability(AapDevice& device) : AapCapability("conversationAwareness", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapConversationAwarenessMode mode){
            bool newOption = mode == AapConversationAwarenessMode::On;

            if (!isAvailable)
            {
                isAvailable = true;
                option = newOption;
                Logger::Info("ConversationAwareness updated: %s", AapConversationAwarenessModeToString(mode).c_str());
                _onChanged.FireEvent(*this);
            }
            else if (option != newOption)
            {
                option = newOption;
                Logger::Info("ConversationAwareness updated: %s", AapConversationAwarenessModeToString(mode).c_str());
                _onChanged.FireEvent(*this);
            }
        });

    }

    AapConversationAwarenessCapability::~AapConversationAwarenessCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapConversationAwarenessCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_object())
        {
            const auto& selected = capability.at("selected");
            if (!selected.contains("enabled") || !selected["enabled"].is_boolean() ||
                !selected.contains("volume") || !selected["volume"].is_number_integer())
            {
                Logger::Error("Error: AapConversationAwarenessCapability::SetFromJson requires enabled and volume");
                return;
            }

            int64_t volume = selected["volume"].get<int64_t>();
            if (volume < 0 || volume > 100)
            {
                Logger::Error("Error: AapConversationAwarenessCapability::SetFromJson volume must be between 0 and 100");
                return;
            }

            bool enabled = selected["enabled"].get<bool>();
            AapConversationAwarenessMode nativeMode = enabled ? AapConversationAwarenessMode::On : AapConversationAwarenessMode::Off;

            SendData(AapSetConversationAwarenessStatus(nativeMode));
            SaveVolumeSetting(static_cast<int>(volume));

            // AirPods do not send a notification after this setting is changed.
            isAvailable = true;
            option = enabled;
            if (!enabled)
                device.StopVolumeReduction(AapVolumeReductionReason::ConversationAwareness);

            _onChanged.FireEvent(*this);
            Logger::Info("ConversationAwareness updated: %s", AapConversationAwarenessModeToString(nativeMode).c_str());
            Logger::Debug("AapConversationAwarenessCapability::SetFromJson set enabled to %s", enabled ? "true" : "false");
            Logger::Debug("AapConversationAwarenessCapability::SetFromJson set volume to %d", static_cast<int>(volume));
        }
        else
        {
            Logger::Error("Error: AapConversationAwarenessCapability::SetFromJson got no selected object");
        }
    }
}
