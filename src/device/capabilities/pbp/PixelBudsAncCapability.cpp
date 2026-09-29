// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsAncCapability.h"
#include "sdk/pbp/setters/PixelBudsSetAnc.h"

namespace MagicPodsCore
{
    PixelBudsAncState PixelBudsAncCapability::DeviceAncModesToPixelBudsAncState(DeviceAncModes mode)
    {
        switch (mode)
        {
        case DeviceAncModes::Off:
            return PixelBudsAncState::Off;
        case DeviceAncModes::Transparency:
            return PixelBudsAncState::Aware;

        case DeviceAncModes::WindCancellation: // Wind cancellation is available for Sony headphones only. It will 100% not be used for any other brand, but to avoid unexpected issues, we have included it here
            return PixelBudsAncState::Active;

        case DeviceAncModes::NoiseCancellation:
            return PixelBudsAncState::Active;
        case DeviceAncModes::Adaptive:
            return PixelBudsAncState::Adaptive;

        // It will never be called, but it is added to avoid a build warning.
        default:
            return PixelBudsAncState::Off;
        }
    }
    DeviceAncModes PixelBudsAncCapability::PixelBudsAncStateToDeviceAncModes(PixelBudsAncState state)
    {
        switch (state)
        {
        case PixelBudsAncState::Off:
            return DeviceAncModes::Off;
        case PixelBudsAncState::Aware:
            return DeviceAncModes::Transparency;
        case PixelBudsAncState::Active:
            return DeviceAncModes::NoiseCancellation;
        case PixelBudsAncState::Adaptive:
            return DeviceAncModes::Adaptive;

        // It will never be called, but it is added to avoid a build warning.
        default:
            return DeviceAncModes::Off;
        }
    }

    nlohmann::json PixelBudsAncCapability::CreateJsonBody()
    {
        std::vector<PixelBudsAncState> states = watcher.GetAncModesFor(static_cast<PixelBudsModelIds>(device.GetProductId()));

        unsigned char optionsFlag = 0;
        for (auto state : states)
        {
            optionsFlag |= static_cast<unsigned char>(PixelBudsAncStateToDeviceAncModes(state));
        }

        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        bodyJson["options"] = optionsFlag;
        return bodyJson;
    }

    void PixelBudsAncCapability::OnReceivedData(const PwRpcPacket &packet)
    {
        watcher.ProcessResponse(packet);
    }

    PixelBudsAncCapability::PixelBudsAncCapability(PixelBudsDevice& device) : PixelBudsCapability("anc", false, device),
                                                                             watcher(PixelBudsAncWatcher(static_cast<PixelBudsModelIds>(device.GetProductId())))
    {
        watcherAncChangedEventId = watcher.GetAncChangedEvent().Subscribe([this](size_t id, PixelBudsAncState state){
            DeviceAncModes newOption = PixelBudsAncStateToDeviceAncModes(state);
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

    PixelBudsAncCapability::~PixelBudsAncCapability(){
        watcher.GetAncChangedEvent().Unsubscribe(watcherAncChangedEventId);
    }

    void PixelBudsAncCapability::SetFromJson(const nlohmann::json &json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_number_integer())
        {
            unsigned char selected = static_cast<unsigned char>(capability["selected"].get<int>());
            if (isValidDeviceAncModesType(selected))
            {
                PixelBudsAncState nativeState = DeviceAncModesToPixelBudsAncState(static_cast<DeviceAncModes>(selected));
                SendData(PixelBudsSetAnc(nativeState));
                Logger::Debug("PixelBudsAncCapability::SetFromJson set option to %s", PixelBudsAncStateToString(nativeState).c_str());
            }
            else
            {
                Logger::Info("Error: PixelBudsAncCapability::SetFromJson got unexpected option: %d", selected);
            }
        }
        else
        {
            Logger::Info("Error: PixelBudsAncCapability::SetFromJson got no value or value is not an integer");
        }
    }

}
