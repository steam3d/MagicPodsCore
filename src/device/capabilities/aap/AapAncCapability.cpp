#include "AapAncCapability.h"

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
        bodyJson["Value"] = option;
        bodyJson["Options"] = optionsFlag;
        return bodyJson;
    }

    void AapAncCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapAncCapability::AapAncCapability(AapDevice &device) : AapCapability("anc", false, device)
    {
        watcherAncChangedEventId = watcher.GetEvent().Subscribe([this](size_t id, AapAncMode mode){
            DeviceAncModes newOption = AapAncModeToDeviceAncModes(mode);
            if (!isAvailable){
                    //When we get ANC mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                    isAvailable = true;
                    option = newOption;
                    _onChanged.FireEvent(*this);
                }
                else if (option != newOption){
                    option = newOption;
                    _onChanged.FireEvent(*this);
                }
        });

    }

    AapAncCapability::~AapAncCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherAncChangedEventId);
    }

    void AapAncCapability::SetFromJson(nlohmann::json json)
    {
        // {"value":1}
        if (json.contains("value") && json["value"].is_number_integer())
        {
            unsigned char value = static_cast<unsigned char>(json["value"].get<int>());
            if (isValidDeviceAncModesType(value))
            {
                AapAncMode nativeMode = DeviceAncModesToAapAncMode(static_cast<DeviceAncModes>(value));
                SendData(AapSetAnc(nativeMode));
                LOG_DEBUG("AapAncCapability::SetFromJson set option to %s", AapAncModeToString(nativeMode).c_str());
            }
            else
            {
                LOG_RELEASE("Error: AapAncCapability::SetFromJson got unexpected option: %d", value);
            }
        }
        else
        {
            LOG_RELEASE("Error: AapAncCapability::SetFromJson got no value or value is not an integer");
        }
    }
}
