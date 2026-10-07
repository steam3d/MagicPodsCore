// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapEarDetectionCapability.h"

namespace MagicPodsCore
{
    DeviceEarDetectionData AapEarDetectionCapability::AapEarDetectionCapabilityToDeviceEarDetectionData(AapEarDetectionArgs mode)
    {
        if (mode.EarOne == AapEarDetectionState::_InClosedCase && mode.EarTwo == AapEarDetectionState::_InClosedCase)
        {
            return DeviceEarDetectionData(DeviceEarStatus::NotAvailable);
        }

        // one active
        if (mode.EarOne == AapEarDetectionState::_InClosedCase && mode.EarTwo != AapEarDetectionState::_InClosedCase)
        {
            return DeviceEarDetectionData(mode.EarTwo == AapEarDetectionState::InEar ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
        }

        if (mode.EarOne != AapEarDetectionState::_InClosedCase && mode.EarTwo == AapEarDetectionState::_InClosedCase)
        {
            return DeviceEarDetectionData(mode.EarOne == AapEarDetectionState::InEar ? DeviceEarStatus::Worn : DeviceEarStatus::NotWorn);
        }

        // both
        if (mode.EarOne == AapEarDetectionState::InEar && mode.EarTwo == AapEarDetectionState::InEar)
        {
            return DeviceEarDetectionData(DeviceEarStatus::Worn);
        }

        if (mode.EarOne != AapEarDetectionState::InEar && mode.EarTwo != AapEarDetectionState::InEar)
        {
            return DeviceEarDetectionData(DeviceEarStatus::NotWorn);
        }

        // one out
        if (mode.EarOne == AapEarDetectionState::InEar && mode.EarTwo != AapEarDetectionState::InEar)
        {
            return DeviceEarDetectionData(DeviceEarStatus::PartiallyWorn);
        }

        if (mode.EarOne != AapEarDetectionState::InEar && mode.EarTwo == AapEarDetectionState::InEar)
        {
            return DeviceEarDetectionData(DeviceEarStatus::PartiallyWorn);
        }

        return DeviceEarDetectionData();
    }

    bool AapEarDetectionCapability::LoadSetting()
    {
        std::optional<bool> settingEarDetection = device.LoadSettingBool("ear_detection");
        if (settingEarDetection.has_value())
            return settingEarDetection.value();

        return true;  //default setting
    }

    void AapEarDetectionCapability::SaveSetting(bool b)
    {
        device.SaveSettingBool("ear_detection", b);
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
    // DEMO
    nlohmann::json AapEarDetectionCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = LoadSetting();
        bodyJson["state"] = option.Status;
        return bodyJson;
    }

    void AapEarDetectionCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapEarDetectionCapability::AapEarDetectionCapability(AapDevice& device) : AapCapability("earDetection", false, device)
    {
        watcherAncChangedEventId = watcher.GetEvent().Subscribe([this](size_t id, AapEarDetectionArgs mode){
            DeviceEarDetectionData newOption = AapEarDetectionCapabilityToDeviceEarDetectionData(mode);
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

    AapEarDetectionCapability::~AapEarDetectionCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherAncChangedEventId);
    }

    void AapEarDetectionCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_boolean())
        {
            bool selected = capability["selected"].get<bool>();

            SaveSetting(selected);
            _onChanged.FireEvent(*this);

            Logger::Debug("AapEarDetectionCapability::SetFromJson set option to %s", selected ? "true" : "false");
        }
        else
        {
            Logger::Error("Error: AapEarDetectionCapability::SetFromJson got no value or value is not an boolean");
        }
    }

}
