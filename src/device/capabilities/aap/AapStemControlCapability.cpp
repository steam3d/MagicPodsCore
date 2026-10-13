// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapStemControlCapability.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "sdk/aap/setters/AapSetStemControl.h"

namespace MagicPodsCore
{
    static constexpr auto StemControlSetting = "aap_stem_control";

    nlohmann::json AapStemControlCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        return bodyJson;
    }

    void AapStemControlCapability::OnReceivedData(const std::vector<unsigned char> &)
    {
    }

    AapStemControlCapability::AapStemControlCapability(AapDevice& device) : AapCapability("stemControl", false, device)
    {
        clientStateChangedEventId = device.GetClientStateChangedEvent().Subscribe([this](size_t, ClientState state){
            if (state == ClientState::Connected){
                option = 0;
                const auto setting = this->device.LoadSettingInt(StemControlSetting);
                if (setting.has_value() && setting.value() >= 0 && setting.value() <= 0x0f)
                    option = static_cast<unsigned char>(setting.value());
                else if (setting.has_value())
                    Logger::Error("StemControl setting has unexpected value: %lld", static_cast<long long>(setting.value()));

                // Stem control is disabled by default, so there is no need to send the default state.
                if (option != 0)
                    SendData(AapSetStemControl(option));
                isAvailable = true;
                Logger::Info("StemControl updated: %u", static_cast<unsigned int>(option));
                _onChanged.FireEvent(*this);
            }
        });
    }

    AapStemControlCapability::~AapStemControlCapability()
    {
        device.GetClientStateChangedEvent().Unsubscribe(clientStateChangedEventId);
    }

    bool AapStemControlCapability::IsSupported(unsigned short model)
    {
        switch (static_cast<AapModelIds>(model))
        {
        case AapModelIds::airpodspro2:
        case AapModelIds::airpodspro3:
        case AapModelIds::airpodsprousbc:
        case AapModelIds::airpods4:
        case AapModelIds::airpods4anc:
        case AapModelIds::airpods5:
        case AapModelIds::airpods5wcc:
        //case AapModelIds::airpodsmax2: must be tested befor enabling
            return true;
        default:
            return false;
        }
    }

    void AapStemControlCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_number_integer())
        {
            Logger::Error("Error: AapStemControlCapability::SetFromJson got no value or value is not an integer");
            return;
        }

        int selected = capability["selected"].get<int>();
        if (selected < 0 || selected > 0x0f)
        {
            Logger::Error("Error: AapStemControlCapability::SetFromJson got unexpected option: %d", selected);
            return;
        }

        unsigned char modes = static_cast<unsigned char>(selected);

        SendData(AapSetStemControl(modes));
        device.SaveSettingInt(StemControlSetting, modes);

        if (option != modes){
            option = modes;
            Logger::Info("StemControl updated: %u", static_cast<unsigned int>(option));
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("AapStemControlCapability::SetFromJson set option to %u", static_cast<unsigned int>(option));
    }
}
