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
        // bodyJson["selected"]["lowerVolume"] = false;
        // bodyJson["selected"]["switchToSpeakers"] = false;
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
                TriggerMediaControl();
                _onChanged.FireEvent(*this);
            }
            else if (!option.AreStatesEqual(newOption)){
                option = newOption;
                Logger::Info("EarDetection updated: %s", option.ToString().c_str());
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
            if (!selected.contains("pauseMedia") || !selected["pauseMedia"].is_boolean())
            {
                Logger::Error("Error: AapEarDetectionCapability::SetFromJson got no pauseMedia value or value is not a boolean");
                return;
            }

            bool pauseMedia = selected["pauseMedia"].get<bool>();

            SaveSetting(pauseMedia);
            _onChanged.FireEvent(*this);

            Logger::Debug("AapEarDetectionCapability::SetFromJson set pauseMedia to %s", pauseMedia ? "true" : "false");
        }
        else
        {
            Logger::Error("Error: AapEarDetectionCapability::SetFromJson got no selected object");
        }
    }

}
