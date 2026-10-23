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

    int AapEarDetectionCapability::LoadLowerVolumeSetting()
    {
        std::optional<int64_t> setting = device.LoadSettingInt("ear_lower_volume");
        if (!setting.has_value() || setting.value() <= 0)
            return 0;
        if (setting.value() > 100)
            return 100;

        return static_cast<int>(setting.value());
    }

    void AapEarDetectionCapability::SaveLowerVolumeSetting(int value)
    {
        device.SaveSettingInt("ear_lower_volume", value);
    }

    void AapEarDetectionCapability::TriggerAudioOutputSwitch()
    {
        if (!LoadSwitchToSpeakersSetting())
            return;

        auto audioClient = device.GetAudioClient();
        auto card = audioClient->GetCardInfoByName(audioClient->GetNameFromMac(device.GetAddress()));
        if (!card.has_value() || !card->activeProfile.starts_with("a2dp-sink"))
            return;

        if (option.Status == DeviceEarStatus::NotWorn)
        {
            audioClient->SwitchCardToSpeakers(card.value());
            return;
        }

        bool headphonesAreWorn = option.Status == DeviceEarStatus::Worn ||
                                 option.Status == DeviceEarStatus::PartiallyWorn;
        if (!headphonesAreWorn)
            return;

        audioClient->SwitchToCard(card.value());
    }

    void AapEarDetectionCapability::TriggerMediaControl(DeviceEarStatus previousStatus)
    {
        if (LoadSetting() == false)
            return;

        if (option.Status == DeviceEarStatus::Worn)
        {
            device.PlayMedia();
            return;
        }

        if (option.Status == DeviceEarStatus::NotWorn)
        {
            device.PauseMedia();
            return;
        }

        if (option.Status != DeviceEarStatus::PartiallyWorn)
            return;

        if (previousStatus == DeviceEarStatus::NotWorn)
            device.PlayMedia();
        else if (previousStatus == DeviceEarStatus::Worn)
            device.PauseMedia();
    }

    void AapEarDetectionCapability::TriggerVolumeControl(DeviceEarStatus previousStatus)
    {
        if (previousStatus == DeviceEarStatus::Worn &&
            option.Status == DeviceEarStatus::PartiallyWorn)
        {
            device.StartVolumeReduction(AapVolumeReductionReason::EarDetection, LoadLowerVolumeSetting());
            return;
        }

        bool headphonesAreWorn = option.Status == DeviceEarStatus::Worn ||
                                 (previousStatus == DeviceEarStatus::NotWorn &&
                                  option.Status == DeviceEarStatus::PartiallyWorn);
        if (headphonesAreWorn)
            device.StopVolumeReduction(AapVolumeReductionReason::EarDetection);
    }

    nlohmann::json AapEarDetectionCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = nlohmann::json::object();
        bodyJson["selected"]["pauseMedia"] = LoadSetting();
        bodyJson["selected"]["switchToSpeakers"] = LoadSwitchToSpeakersSetting();
        bodyJson["selected"]["lowerVolume"] = LoadLowerVolumeSetting();
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
                DeviceEarStatus previousStatus = option.Status;
                isAvailable = true;
                option = newOption;
                Logger::Info("EarDetection updated: %s", option.ToString().c_str());
                TriggerAudioOutputSwitch();
                TriggerMediaControl(previousStatus);
                TriggerVolumeControl(previousStatus);
                _onChanged.FireEvent(*this);
            }
            else if (!option.AreStatesEqual(newOption)){
                DeviceEarStatus previousStatus = option.Status;
                option = newOption;
                Logger::Info("EarDetection updated: %s", option.ToString().c_str());
                TriggerAudioOutputSwitch();
                TriggerMediaControl(previousStatus);
                TriggerVolumeControl(previousStatus);
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
            if (!selected.contains("pauseMedia") || !selected["pauseMedia"].is_boolean() ||
                !selected.contains("switchToSpeakers") || !selected["switchToSpeakers"].is_boolean() ||
                !selected.contains("lowerVolume") || !selected["lowerVolume"].is_number_integer())
            {
                Logger::Error("Error: AapEarDetectionCapability::SetFromJson requires pauseMedia, switchToSpeakers and lowerVolume");
                return;
            }

            int64_t lowerVolume = selected["lowerVolume"].get<int64_t>();
            if (lowerVolume < 0 || lowerVolume > 100)
            {
                Logger::Error("Error: AapEarDetectionCapability::SetFromJson lowerVolume must be between 0 and 100");
                return;
            }

            bool pauseMedia = selected["pauseMedia"].get<bool>();
            bool switchToSpeakers = selected["switchToSpeakers"].get<bool>();

            SaveSetting(pauseMedia);
            SaveSwitchToSpeakersSetting(switchToSpeakers);
            SaveLowerVolumeSetting(static_cast<int>(lowerVolume));
            _onChanged.FireEvent(*this);

            Logger::Debug("AapEarDetectionCapability::SetFromJson set pauseMedia to %s", pauseMedia ? "true" : "false");
            Logger::Debug("AapEarDetectionCapability::SetFromJson set switchToSpeakers to %s", switchToSpeakers ? "true" : "false");
            Logger::Debug("AapEarDetectionCapability::SetFromJson set lowerVolume to %d", static_cast<int>(lowerVolume));
        }
        else
        {
            Logger::Error("Error: AapEarDetectionCapability::SetFromJson got no selected object");
        }
    }

}
