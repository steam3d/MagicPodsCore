// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsEarDetectionCapability.h"
#include "sdk/sgb/GalaxyBudsPacket.h"
#include "device/enums/DeviceEarStatus.h"

namespace MagicPodsCore
{
    DeviceEarDetectionData GalaxyBudsEarDetectionCapability::GalaxyBudsEarDetectionToDeviceEarDetectionData(GalaxyBudsEarDetectionStateArgs mode)
    {
        if (mode.Left == GalaxyBudsEarDetectionState::Disconnected && mode.Right == GalaxyBudsEarDetectionState::Disconnected)
        {
            return DeviceEarDetectionData(DeviceEarStatus::NotAvailable);
        }

        // one active
        if (mode.Left == GalaxyBudsEarDetectionState::Disconnected && mode.Right != GalaxyBudsEarDetectionState::Disconnected)
        {
            return DeviceEarDetectionData(mode.Right == GalaxyBudsEarDetectionState::Wearing ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
        }

        if (mode.Left != GalaxyBudsEarDetectionState::Disconnected && mode.Right == GalaxyBudsEarDetectionState::Disconnected)
        {
            return DeviceEarDetectionData(mode.Left == GalaxyBudsEarDetectionState::Wearing ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
        }

        // both
        if (mode.Left == GalaxyBudsEarDetectionState::Wearing && mode.Right == GalaxyBudsEarDetectionState::Wearing)
        {
            return DeviceEarDetectionData(DeviceEarStatus::Worn);
        }

        if (mode.Left != GalaxyBudsEarDetectionState::Wearing && mode.Right != GalaxyBudsEarDetectionState::Wearing)
        {
            return DeviceEarDetectionData(DeviceEarStatus::NotWorn);
        }

        // one out
        if (mode.Left == GalaxyBudsEarDetectionState::Wearing && mode.Right != GalaxyBudsEarDetectionState::Wearing)
        {
            return DeviceEarDetectionData(DeviceEarStatus::PartiallyWorn);
        }

        if (mode.Left != GalaxyBudsEarDetectionState::Wearing && mode.Right == GalaxyBudsEarDetectionState::Wearing)
        {
            return DeviceEarDetectionData(DeviceEarStatus::PartiallyWorn);
        }

        return DeviceEarDetectionData();
    }

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
        std::optional<bool> settingEarDetection = device.LoadSettingBool("ear_detection");
        if (settingEarDetection.has_value())
            return settingEarDetection.value();

        return true;  //default setting
    }

    void GalaxyBudsEarDetectionCapability::SaveSetting(bool b)
    {
        device.SaveSettingBool("ear_detection", b);
    }

    nlohmann::json GalaxyBudsEarDetectionCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = LoadSetting();
        bodyJson["state"] = option.Status;
        return bodyJson;
    }

    void GalaxyBudsEarDetectionCapability::OnReceivedData(const GalaxyBudsResponseData &data)
    {
        watcher.ProcessResponse(data);
    }

    GalaxyBudsEarDetectionCapability::GalaxyBudsEarDetectionCapability(GalaxyBudsDevice& device) : GalaxyBudsCapability("earDetection", false, device),
                                                                                                           watcher(GalaxyBudsEarDetectionWatcher(static_cast<GalaxyBudsModelIds>(device.GetProductId())))
    {
        watcherEarChangedEventId = watcher.GetEarDetectionStateChangedEvent().Subscribe([this](size_t id, GalaxyBudsEarDetectionStateArgs mode){

            DeviceEarDetectionData newOption = GalaxyBudsEarDetectionToDeviceEarDetectionData(mode);
            if (!isAvailable){
                //When we get ANC mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
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
                _onChanged.FireEvent(*this);
            }
        });
    }

    GalaxyBudsEarDetectionCapability::~GalaxyBudsEarDetectionCapability(){
        watcher.GetEarDetectionStateChangedEvent().Unsubscribe(watcherEarChangedEventId);
    }

    void GalaxyBudsEarDetectionCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_boolean())
        {
            bool selected = capability["selected"].get<bool>();

            SaveSetting(selected);
            _onChanged.FireEvent(*this);

            Logger::Debug("GalaxyBudsEarDetectionCapability::SetFromJson set option to %s", selected ? "true" : "false");
        }
        else
        {
            Logger::Error("Error: GalaxyBudsEarDetectionCapability::SetFromJson got no value or value is not an boolean");
        }
    }
}
