// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapEarDetectionCapability.h"
#include "AapEarDetectionHelper.h"

namespace MagicPodsCore
{
    bool AapEarDetectionCapability::LoadSetting()
    {
        std::optional<bool> settingEarDetection = device.LoadSettingBool("ear_pause_media");
        if (settingEarDetection.has_value())
            return settingEarDetection.value();

        return true;  //default setting
    }

    void AapEarDetectionCapability::SaveSetting(bool b)
    {
        device.SaveSettingBool("ear_pause_media", b);
    }

    bool AapEarDetectionCapability::LoadSwitchToSpeakersSetting()
    {
        std::optional<bool> setting = device.LoadSettingBool("ear_switch_to_speakers");
        if (setting.has_value())
            return setting.value();

        return false;
    }

    void AapEarDetectionCapability::SaveSwitchToSpeakersSetting(bool b)
    {
        device.SaveSettingBool("ear_switch_to_speakers", b);
    }

    void AapEarDetectionCapability::TriggerAudioOutputSwitch()
    {
        if (!LoadSwitchToSpeakersSetting())
        {
            switchedToSpeakers = false;
            speakersSinkName.clear();
            return;
        }

        auto audioClient = device.GetAudioClient();

        if (option.Status == DeviceEarStatus::NotWorn)
        {
            auto card = audioClient->GetCardInfoByName(audioClient->GetNameFromMac(device.GetAddress()));
            if (!card.has_value() || !card->activeProfile.starts_with("a2dp-sink"))
                return;

            auto defaultSinkName = audioClient->GetDefaultSinkName();
            if (!defaultSinkName.has_value())
                return;

            auto headphonesSink = audioClient->GetSinkInfoByName(defaultSinkName.value());
            if (!headphonesSink.has_value() || headphonesSink->card != card->index)
                return;

            for (const auto& sink : audioClient->GetSinks())
            {
                if (sink.card == card->index || sink.activePort.find("speaker") == std::string::npos)
                    continue;

                if (!audioClient->SetDefaultSink(sink.name))
                    return;

                if (!audioClient->MoveSinkInputs(headphonesSink->index, sink.index))
                    Logger::Warn("Failed to move some audio streams to speakers");
                speakersSinkName = sink.name;
                switchedToSpeakers = true;
                Logger::Info("Audio output switched to speakers: %s", sink.name.c_str());
                return;
            }

            Logger::Warn("Failed to find speakers audio output");
            return;
        }

        bool headphonesAreWorn = option.Status == DeviceEarStatus::Worn ||
                                 option.Status == DeviceEarStatus::PartiallyWorn;
        if (!headphonesAreWorn || !switchedToSpeakers)
            return;

        auto defaultSinkName = audioClient->GetDefaultSinkName();
        if (!defaultSinkName.has_value())
            return;

        if (defaultSinkName.value() != speakersSinkName)
        {
            switchedToSpeakers = false;
            speakersSinkName.clear();
            return;
        }

        auto speakersSink = audioClient->GetSinkInfoByName(speakersSinkName);
        auto card = audioClient->GetCardInfoByName(audioClient->GetNameFromMac(device.GetAddress()));
        if (!speakersSink.has_value() || !card.has_value() || !card->activeProfile.starts_with("a2dp-sink"))
            return;

        for (const auto& sink : audioClient->GetSinks())
        {
            if (sink.card != card->index)
                continue;

            if (!audioClient->SetDefaultSink(sink.name))
                return;

            if (!audioClient->MoveSinkInputs(speakersSink->index, sink.index))
                Logger::Warn("Failed to move some audio streams to headphones");
            switchedToSpeakers = false;
            speakersSinkName.clear();
            Logger::Info("Audio output switched to headphones: %s", sink.name.c_str());
            return;
        }
    }

    void AapEarDetectionCapability::TriggerMediaControl()
    {
        if (LoadSetting() == false)
            return;

        DeviceEarMediaStatus status = option.GetMediaStatus();

        if (status != DeviceEarMediaStatus::NotAvailable){
            if (status == DeviceEarMediaStatus::Play){
                device.PlayMedia();
            }
            else{
                device.PauseMedia();
            }
        }
    }
    nlohmann::json AapEarDetectionCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = nlohmann::json::object();
        bodyJson["selected"]["pauseMedia"] = LoadSetting();
        bodyJson["selected"]["switchToSpeakers"] = LoadSwitchToSpeakersSetting();
        // bodyJson["selected"]["lowerVolume"] = false;
        return bodyJson;
    }

    void AapEarDetectionCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapEarDetectionCapability::AapEarDetectionCapability(AapDevice& device) : AapCapability("earDetection", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapEarDetectionArgs state){
            DeviceEarDetectionData newOption = AapEarDetectionHelper::ToDeviceEarDetectionData(state);
            if (!isAvailable){
                isAvailable = true;
                option = newOption;
                Logger::Info("EarDetection updated: %s", option.ToString().c_str());
                TriggerAudioOutputSwitch();
                TriggerMediaControl();
                _onChanged.FireEvent(*this);
            }
            else if (!option.AreStatesEqual(newOption)){
                option = newOption;
                Logger::Info("EarDetection updated: %s", option.ToString().c_str());
                TriggerAudioOutputSwitch();
                TriggerMediaControl();
            }
        });

    }

    AapEarDetectionCapability::~AapEarDetectionCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapEarDetectionCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_object())
        {
            const auto& selected = capability.at("selected");
            bool changed = false;

            if (selected.contains("pauseMedia") && selected["pauseMedia"].is_boolean())
            {
                bool pauseMedia = selected["pauseMedia"].get<bool>();
                SaveSetting(pauseMedia);
                changed = true;
                Logger::Debug("AapEarDetectionCapability::SetFromJson set pauseMedia to %s", pauseMedia ? "true" : "false");
            }

            if (selected.contains("switchToSpeakers") && selected["switchToSpeakers"].is_boolean())
            {
                bool switchToSpeakers = selected["switchToSpeakers"].get<bool>();
                SaveSwitchToSpeakersSetting(switchToSpeakers);
                changed = true;
                Logger::Debug("AapEarDetectionCapability::SetFromJson set switchToSpeakers to %s", switchToSpeakers ? "true" : "false");
            }

            if (changed)
                _onChanged.FireEvent(*this);
            else
                Logger::Error("Error: AapEarDetectionCapability::SetFromJson got no supported values");
        }
        else
        {
            Logger::Error("Error: AapEarDetectionCapability::SetFromJson got no selected object");
        }
    }

}
