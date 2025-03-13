#include "GalaxyBudsAncWatcher.h"

namespace MagicPodsCore{

    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::ExtractState(unsigned char b)
    {
        if (model == GalaxyBudsModelIds::GalaxyBudsLive)
        {
            return b == 1 ? GalaxyBudsAnc::NoiseReduction : GalaxyBudsAnc::Off; // Unknown usage
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPro ||
                 model == GalaxyBudsModelIds::GalaxyBuds2 ||
                 model == GalaxyBudsModelIds::GalaxyBuds2Pro ||
                 model == GalaxyBudsModelIds::GalaxyBudsFe ||
                 model == GalaxyBudsModelIds::GalaxyBuds3 ||
                 model == GalaxyBudsModelIds::GalaxyBuds3Pro)
        {
            if (isValidGalaxyBudsAncType(b))
            {
                return static_cast<GalaxyBudsAnc>(b);
            }
        }

        return std::nullopt;
    }

    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::Process_EXTENDED_STATUS_UPDATED(GalaxyBudsResponseData data)
    {
        if (data.Id != GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED)
            return std::nullopt;

        if (data.Payload.size() <= 12)
            return std::nullopt;

        return ExtractState(data.Payload[12]);
    }

    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::Process_NOISE_CONTROLS_UPDATE(GalaxyBudsResponseData data)
    {
        if (data.Id != GalaxyBudsMsgIds::NOISE_CONTROLS_UPDATE)
            return std::nullopt;

        if (data.Payload.size() <= 0)
            return std::nullopt;

        return ExtractState(data.Payload[0]);

        // unused
        // if (data.Payload.Length > 1)
        //{
        //    var WearingState = (GalaxyBudsLegacyEarDetectionState)data.Payload[1];
        //}
    }
    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::Process_UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT(GalaxyBudsResponseData data)
    {
        if (data.Id != GalaxyBudsMsgIds::UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT)
            return std::nullopt;

        if (data.Payload.size() <= 1)
            return std::nullopt;

        if (data.Payload[0] != static_cast<unsigned char>(GalaxyBudsMsgIds::NOISE_CONTROLS))            
            return std::nullopt;

        return ExtractState(data.Payload[1]);
    }

    void GalaxyBudsAncWatcher::ProcessResponse(GalaxyBudsResponseData data)
    {
        std::optional<GalaxyBudsAnc> stateOpt;

        if (data.Id == GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED)
        {
            stateOpt = Process_EXTENDED_STATUS_UPDATED(data);
        }
        else if (data.Id == GalaxyBudsMsgIds::NOISE_CONTROLS_UPDATE)
        {
            stateOpt = Process_NOISE_CONTROLS_UPDATE(data);
        }
        else if (data.Id == GalaxyBudsMsgIds::UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT)
        {
            stateOpt = Process_UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT(data);
        }

        if (stateOpt.has_value())
        {
            GalaxyBudsAnc state = stateOpt.value();
                _ancChanged.FireEvent(state);            
        }
    }

    bool GalaxyBudsAncWatcher::IsSupport(GalaxyBudsModelIds model)
    {
        return (model == GalaxyBudsModelIds::GalaxyBudsLive ||
                model == GalaxyBudsModelIds::GalaxyBudsPro ||
                model == GalaxyBudsModelIds::GalaxyBuds2 ||
                model == GalaxyBudsModelIds::GalaxyBuds2Pro ||
                model == GalaxyBudsModelIds::GalaxyBudsFe ||
                model == GalaxyBudsModelIds::GalaxyBuds3 ||
                model == GalaxyBudsModelIds::GalaxyBuds3Pro);
    }

    std::vector<GalaxyBudsAnc> GalaxyBudsAncWatcher::GetAncModesFor(GalaxyBudsModelIds model)
    {
        if (model == GalaxyBudsModelIds::GalaxyBudsLive ||
            model == GalaxyBudsModelIds::GalaxyBuds3)
        {
            return std::vector<GalaxyBudsAnc>{
                GalaxyBudsAnc::Off,
                GalaxyBudsAnc::NoiseReduction,
            };
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPro ||
                 model == GalaxyBudsModelIds::GalaxyBuds2)
        {
            return std::vector<GalaxyBudsAnc>{
                GalaxyBudsAnc::Off,
                GalaxyBudsAnc::AmbientSound,
                GalaxyBudsAnc::NoiseReduction,
            };
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds2Pro ||
                 model == GalaxyBudsModelIds::GalaxyBudsFe ||
                 model == GalaxyBudsModelIds::GalaxyBuds3Pro)
        {
            return std::vector<GalaxyBudsAnc>{
                GalaxyBudsAnc::Off,
                GalaxyBudsAnc::AmbientSound,
                GalaxyBudsAnc::Adaptive,
                GalaxyBudsAnc::NoiseReduction,
            };
        }
        else
        {
            return std::vector<GalaxyBudsAnc>{};
        }
    }
}