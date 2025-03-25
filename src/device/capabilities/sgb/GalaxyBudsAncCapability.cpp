#include "GalaxyBudsAncCapability.h"
#include "sdk/sgb/setters/GalaxyBudsSetAnc.h"
#include "sdk/sgb/GalaxyBudsPacket.h"

namespace MagicPodsCore
{
    GalaxyBudsAnc GalaxyBudsAncCapability::DeviceAncModesToGalaxyBudsAnc(DeviceAncModes mode)
    {
        switch (mode)
        {
        case DeviceAncModes::Off:
            return GalaxyBudsAnc::Off;
        case DeviceAncModes::Transparency:
            return GalaxyBudsAnc::AmbientSound;

        case DeviceAncModes::WindCancellation: // Wind cancellation is available for Sony headphones only. It will 100% not be used for any other brand, but to avoid unexpected issues, we have included it here
            return GalaxyBudsAnc::NoiseReduction;

        case DeviceAncModes::NoiseCancellation:
            return GalaxyBudsAnc::NoiseReduction;
        case DeviceAncModes::Adaptive:
            return GalaxyBudsAnc::Adaptive;

        // It will never be called, but it is added to avoid a build warning.
        default:
            return GalaxyBudsAnc::Off;
        }
    }
    DeviceAncModes GalaxyBudsAncCapability::GalaxyBudsAncToDeviceAncModes(GalaxyBudsAnc mode)
    {
        switch (mode)
        {
        case GalaxyBudsAnc::Off:
            return DeviceAncModes::Off;
        case GalaxyBudsAnc::AmbientSound:
            return DeviceAncModes::Transparency;
        case GalaxyBudsAnc::NoiseReduction:
            return DeviceAncModes::NoiseCancellation;
        case GalaxyBudsAnc::Adaptive:
            return DeviceAncModes::Adaptive;

        // It will never be called, but it is added to avoid a build warning.
        default:
            return DeviceAncModes::Off;
        }
    }

    // DEMO
    nlohmann::json GalaxyBudsAncCapability::CreateJsonBody()
    {
        std::vector<GalaxyBudsAnc> modes = watcher.GetAncModesFor(static_cast<GalaxyBudsModelIds>(device.GetProductId()));

        unsigned char optionsFlag = 0;
        for (auto mode : modes)
        {
            optionsFlag |= static_cast<unsigned char>(GalaxyBudsAncToDeviceAncModes(mode));
        }

        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option;
        bodyJson["options"] = optionsFlag;
        return bodyJson;
    }

    void GalaxyBudsAncCapability::OnReceivedData(const GalaxyBudsResponseData &data)
    {
        watcher.ProcessResponse(data);
    }

    GalaxyBudsAncCapability::GalaxyBudsAncCapability(GalaxyBudsDevice &device) : GalaxyBudsCapability("anc", false, device),
                                                                                                           watcher(GalaxyBudsAncWatcher(static_cast<GalaxyBudsModelIds>(device.GetProductId())))
    {
        watcherAncChangedEventId = watcher.GetAncChangedEvent().Subscribe([this](size_t id, GalaxyBudsAnc mode){
            DeviceAncModes newOption = GalaxyBudsAncToDeviceAncModes(mode);
            if (!isAvailable){
                //When we get ANC mode for the first time, we must notify. But on the second and subsequent times, we should notify only if the option has changed.
                isAvailable = true;
                option = newOption;
                LOG_DEBUG("GalaxyBudsAncCapability: %s", DeviceAncModesToString(option).c_str());
                _onChanged.FireEvent(*this);
            }
            else if (option != newOption){
                option = newOption;
                LOG_DEBUG("GalaxyBudsAncCapability: %s", DeviceAncModesToString(option).c_str());
                _onChanged.FireEvent(*this);
            }    
        });
    }

    GalaxyBudsAncCapability::~GalaxyBudsAncCapability(){
        watcher.GetAncChangedEvent().Unsubscribe(watcherAncChangedEventId);
    }

    void GalaxyBudsAncCapability::SetFromJson(const nlohmann::json &json)
    {                
        if (!json.contains(name))
            return;
        
        const auto& capability = json.at(name);

        if (capability.contains("selected") && capability["selected"].is_number_integer())
        {
            unsigned char selected = static_cast<unsigned char>(capability["selected"].get<int>());
            if (isValidDeviceAncModesType(selected))
            {
                GalaxyBudsAnc nativeMode = DeviceAncModesToGalaxyBudsAnc(static_cast<DeviceAncModes>(selected));
                SendData(GalaxyBudsSetAnc(nativeMode));
                LOG_DEBUG("GalaxyBudsAncCapability::SetFromJson set option to %s", GalaxyBudsAncToString(nativeMode).c_str());
            }
            else
            {
                LOG_RELEASE("Error: GalaxyBudsAncCapability::SetFromJson got unexpected option: %d", selected);
            }
        }
        else
        {
            LOG_RELEASE("Error: GalaxyBudsAncCapability::SetFromJson got no value or value is not an integer");
        }
    }

}
