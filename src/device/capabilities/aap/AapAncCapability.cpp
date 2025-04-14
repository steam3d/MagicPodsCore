// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapAncCapability.h"
#include "sdk/aap/setters/AapSetAnc.h"

namespace MagicPodsCore
{
    AapAncMode AapAncCapability::DeviceAncModesToAapAncMode(DeviceAncModes mode)
    {
        switch (mode)
        {
        case DeviceAncModes::Off:
            return AapAncMode::Off;
        case DeviceAncModes::Transparency:
            return AapAncMode::Transparency;

        case DeviceAncModes::WindCancellation: // Wind cancellation is available for Sony headphones only. It will 100% not be used for any other brand, but to avoid unexpected issues, we have included it here
            return AapAncMode::Anc;

        case DeviceAncModes::NoiseCancellation:
            return AapAncMode::Anc;
        case DeviceAncModes::Adaptive:
            return AapAncMode::Adaptive;

        // It will never be called, but it is added to avoid a build warning.
        default:
            return AapAncMode::Off;
        }
    }

    DeviceAncModes AapAncCapability::AapAncModeToDeviceAncModes(AapAncMode mode)
    {
        switch (mode)
        {
        case AapAncMode::Off:
            return DeviceAncModes::Off;
        case AapAncMode::Transparency:
            return DeviceAncModes::Transparency;
        case AapAncMode::Anc:
            return DeviceAncModes::NoiseCancellation;
        case AapAncMode::Adaptive:
            return DeviceAncModes::Adaptive;

        // It will never be called, but it is added to avoid a build warning.
        default:
            return DeviceAncModes::Off;
        }
    }

    // DEMO
    nlohmann::json AapAncCapability::CreateJsonBody()
    {
        std::vector<AapAncMode> modes = AapSetAnc::GetAncModesFor(device.GetProductId());        

        unsigned char optionsFlag = 0;
        for (auto mode : modes)
        {
            optionsFlag |= static_cast<unsigned char>(AapAncModeToDeviceAncModes(mode));
        }

        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        bodyJson["options"] = optionsFlag;
        return bodyJson;
    }

    void AapAncCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapAncCapability::AapAncCapability(AapDevice& device) : AapCapability("anc", false, device)
    {
        watcherAncChangedEventId = watcher.GetEvent().Subscribe([this](size_t id, AapAncMode mode){
            DeviceAncModes newOption = AapAncModeToDeviceAncModes(mode);
            if (!isAvailable){
                    //When we get ANC mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = newOption;
                    Logger::Info("ANC updated: %s", DeviceAncModesToString(option).c_str());
                    _onChanged.FireEvent(*this);
                }
                else if (option != newOption){
                    option = newOption;
                    Logger::Info("ANC updated: %s", DeviceAncModesToString(option).c_str());
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapAncCapability::~AapAncCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherAncChangedEventId);
    }

    void AapAncCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_number_integer())
        {
            unsigned char selected = static_cast<unsigned char>(capability["selected"].get<int>());
            if (isValidDeviceAncModesType(selected))
            {
                AapAncMode nativeMode = DeviceAncModesToAapAncMode(static_cast<DeviceAncModes>(selected));
                SendData(AapSetAnc(nativeMode));
                Logger::Debug("AapAncCapability::SetFromJson set option to %s", AapAncModeToString(nativeMode).c_str());
            }
            else
            {
                Logger::Error("Error: AapAncCapability::SetFromJson got unexpected option: %d", selected);
            }
        }
        else
        {
            Logger::Error("Error: AapAncCapability::SetFromJson got no value or value is not an integer");
        }
    }
}
