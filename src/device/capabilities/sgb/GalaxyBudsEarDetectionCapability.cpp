// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsEarDetectionCapability.h"
#include "GalaxyBudsEarDetectionHelper.h"

namespace MagicPodsCore
{
    void GalaxyBudsEarDetectionCapability::TriggerMediaControl()
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

    bool GalaxyBudsEarDetectionCapability::LoadSetting()
    {
        std::optional<bool> settingEarDetection = device.LoadSettingBool("ear_pause_media");
        if (settingEarDetection.has_value())
            return settingEarDetection.value();

        return true;  //default setting
    }

    void GalaxyBudsEarDetectionCapability::SaveSetting(bool b)
    {
        device.SaveSettingBool("ear_pause_media", b);
    }

    nlohmann::json GalaxyBudsEarDetectionCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = nlohmann::json::object();
        bodyJson["selected"]["pauseMedia"] = LoadSetting();
        // bodyJson["selected"]["lowerVolume"] = false;
        // bodyJson["selected"]["switchToSpeakers"] = false;
        return bodyJson;
    }

    void GalaxyBudsEarDetectionCapability::OnReceivedData(const GalaxyBudsResponseData &data)
    {
        watcher.ProcessResponse(data);
    }

    GalaxyBudsEarDetectionCapability::GalaxyBudsEarDetectionCapability(GalaxyBudsDevice& device) : GalaxyBudsCapability("earDetection", false, device),
                                                                                                           watcher(GalaxyBudsEarDetectionWatcher(static_cast<GalaxyBudsModelIds>(device.GetProductId())))
    {
        watcherEventId = watcher.GetEarDetectionStateChangedEvent().Subscribe([this](size_t id, GalaxyBudsEarDetectionStateArgs state){

            DeviceEarDetectionData newOption = GalaxyBudsEarDetectionHelper::ToDeviceEarDetectionData(state);
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

    GalaxyBudsEarDetectionCapability::~GalaxyBudsEarDetectionCapability(){
        watcher.GetEarDetectionStateChangedEvent().Unsubscribe(watcherEventId);
    }

    void GalaxyBudsEarDetectionCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_object())
        {
            const auto& selected = capability.at("selected");
            if (!selected.contains("pauseMedia") || !selected["pauseMedia"].is_boolean())
            {
                Logger::Error("Error: GalaxyBudsEarDetectionCapability::SetFromJson got no pauseMedia value or value is not a boolean");
                return;
            }

            bool pauseMedia = selected["pauseMedia"].get<bool>();

            SaveSetting(pauseMedia);
            _onChanged.FireEvent(*this);

            Logger::Debug("GalaxyBudsEarDetectionCapability::SetFromJson set pauseMedia to %s", pauseMedia ? "true" : "false");
        }
        else
        {
            Logger::Error("Error: GalaxyBudsEarDetectionCapability::SetFromJson got no selected object");
        }
    }
}
