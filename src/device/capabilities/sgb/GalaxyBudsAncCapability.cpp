#include "GalaxyBudsAncCapability.h"

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
        bodyJson["Value"] = option;
        bodyJson["Options"] = optionsFlag;
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
                if (!isAvailable)
                    isAvailable = true;

                option = GalaxyBudsAncToDeviceAncModes(mode);
                _onChanged.FireEvent(*this);
                LOG_DEBUG("GalaxyBudsAncCapability::GetAncChangedEvent"); });
    }

    GalaxyBudsAncCapability::~GalaxyBudsAncCapability(){
        watcher.GetAncChangedEvent().Unsubscribe(watcherAncChangedEventId);
    }

    void GalaxyBudsAncCapability::SetFromJson(nlohmann::json json)
    {
        // {"value":1}
        if (json.contains("value") && json["value"].is_number_integer())
        {
            unsigned char value = static_cast<unsigned char>(json["value"].get<int>());
            if (isValidDeviceAncModesType(value))
            {
                GalaxyBudsAnc nativeMode = DeviceAncModesToGalaxyBudsAnc(static_cast<DeviceAncModes>(value));
                SendData(GalaxyBudsSetAnc(nativeMode));
                LOG_DEBUG("GalaxyBudsAncCapability::SetFromJson set option to %d", GalaxyBudsAncToString(nativeMode));
            }
            else
            {
                LOG_RELEASE("Error: GalaxyBudsAncCapability::SetFromJson got unexpected option: %d", value);
            }
        }
        else
        {
            LOG_RELEASE("Error: GalaxyBudsAncCapability::SetFromJson got no value or value is not an integer");
        }
    }

}
