// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "CmnBluetoothCodecCapability.h"
#include "Logger.h"
#include <chrono>


namespace MagicPodsCore
{

    nlohmann::json CmnBluetoothCodecCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();        
        
        bodyJson["selected"] = info.activeProfile;

        auto options =  nlohmann::json::array();
        for(auto &profile: info.profiles){
            options.push_back({profile.first, profile.second});
        }        
        bodyJson["options"] = options;
        
        return bodyJson;
    }

    void CmnBluetoothCodecCapability::Reset()
    {
        info = {};
        Capability::Reset();
    }

    void CmnBluetoothCodecCapability::UpdateCodecInfo()
    {
        if (!device.GetConnected())
            return;

        auto pac = device.GetAudioClient();
        
        if(auto info = pac->GetCardInfoByName(pac->GetNameFromMac(device.GetAddress()))){
            UpdateCardInfo(*info);
        }
    }

    void CmnBluetoothCodecCapability::UpdateCardInfo(const CardInfo &newinfo)
    {
        if (info != newinfo){
            info = newinfo;

            if (!isAvailable)
                isAvailable = true;

            Logger::Debug("%s, (%s)",info.name.c_str(),info.activeProfile.c_str());
            for(auto &profile: info.profiles){
                Logger::Debug("    %s: %s", profile.first.c_str(), profile.second.c_str());
            }

            _onChanged.FireEvent(*this);        
        }
    }

    bool CmnBluetoothCodecCapability::IsValidSelected(const std::string &selected)
    {
        for(auto &profile: info.profiles){
            if (profile.first == selected)
                return true;            
        } 
        return false;
    }

    CmnBluetoothCodecCapability::CmnBluetoothCodecCapability(Device& device) : Capability("bluetoothCodec", false),
                                                                    device(device)
    {
        onAudioCardPropertyChangedId = device.GetAudioClient()->GatAudioCardPropertyChangedEvent().Subscribe([this](size_t id, const CardInfo& info){
            if (info.name == this->device.GetAudioClient()->GetNameFromMac(this->device.GetAddress())){
                this->UpdateCardInfo(info);
            }
        });
        onConnectedPropertyChangedId = device.GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, const bool b){
            if (!b) {
                Reset();
            }
        });

        if (device.GetConnected()){
            UpdateCodecInfo();
        }
    }

void CmnBluetoothCodecCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_string())
        {
            std::string selected = capability["selected"].get<std::string>();
            if (IsValidSelected(selected))
            {
                auto pac = device.GetAudioClient();
                pac->SetCardProfile(pac->GetNameFromMac(device.GetAddress()), selected);                
                Logger::Debug("CmnBluetoothCodecCapability::SetFromJson set option to %s", selected.c_str());
            }
            else
            {
                Logger::Error("Error: CmnBluetoothCodecCapability::SetFromJson got unexpected option: %s", selected.c_str());
            }
        }
        else
        {
            Logger::Error("Error: CmnBluetoothCodecCapability::SetFromJson got no value or value is not an string");
        }
    }

    CmnBluetoothCodecCapability::~CmnBluetoothCodecCapability()
    {
        device.GetAudioClient()->GatAudioCardPropertyChangedEvent().Unsubscribe(onAudioCardPropertyChangedId);
        device.GetConnectedPropertyChangedEvent().Unsubscribe(onConnectedPropertyChangedId);
    }
}
