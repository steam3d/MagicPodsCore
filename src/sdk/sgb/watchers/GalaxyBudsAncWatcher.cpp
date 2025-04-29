// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsAncWatcher.h"
#include "sdk/sgb/enums/GalaxyBudsMsgIds.h"

// Galaxy Buds Live were disabled due to user feedback that the ANC switch does not work.
namespace MagicPodsCore
{

    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::ExtractState(unsigned char b)
    {
        //if (model == GalaxyBudsModelIds::GalaxyBudsLive)
        //{
        //    return b == 1 ? GalaxyBudsAnc::NoiseReduction : GalaxyBudsAnc::Off; // Unknown usage
        //}
        //else
        if (model == GalaxyBudsModelIds::GalaxyBudsPro ||
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

    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::Process_EXTENDED_STATUS_UPDATED(const GalaxyBudsResponseData &data)
    {
        if (data.Id != GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED)
            return std::nullopt;

        if (data.Payload.size() <= 12)
            return std::nullopt;

        return ExtractState(data.Payload[12]);
    }

    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::Process_NOISE_CONTROLS_UPDATE(const GalaxyBudsResponseData &data)
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
    std::optional<GalaxyBudsAnc> GalaxyBudsAncWatcher::Process_UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT(const GalaxyBudsResponseData &data)
    {
        if (data.Id != GalaxyBudsMsgIds::UNIVERSAL_MSG_ID_ACKNOWLEDGEMENT)
            return std::nullopt;

        if (data.Payload.size() <= 1)
            return std::nullopt;

        if (data.Payload[0] != static_cast<unsigned char>(GalaxyBudsMsgIds::NOISE_CONTROLS))
            return std::nullopt;

        return ExtractState(data.Payload[1]);
    }

    void GalaxyBudsAncWatcher::ProcessResponse(const GalaxyBudsResponseData &data)
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
        //return (model == GalaxyBudsModelIds::GalaxyBudsLive ||
        return (model == GalaxyBudsModelIds::GalaxyBudsPro ||
                model == GalaxyBudsModelIds::GalaxyBuds2 ||
                model == GalaxyBudsModelIds::GalaxyBuds2Pro ||
                model == GalaxyBudsModelIds::GalaxyBudsFe ||
                model == GalaxyBudsModelIds::GalaxyBuds3 ||
                model == GalaxyBudsModelIds::GalaxyBuds3Pro);
    }

    std::vector<GalaxyBudsAnc> GalaxyBudsAncWatcher::GetAncModesFor(GalaxyBudsModelIds model)
    {
        //if (model == GalaxyBudsModelIds::GalaxyBudsLive ||
        if (model == GalaxyBudsModelIds::GalaxyBuds3)
        {
            return std::vector<GalaxyBudsAnc>{
                GalaxyBudsAnc::Off,
                GalaxyBudsAnc::NoiseReduction,
            };
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPro ||
                 model == GalaxyBudsModelIds::GalaxyBuds2 ||
                 model == GalaxyBudsModelIds::GalaxyBuds2Pro ||
                 model == GalaxyBudsModelIds::GalaxyBudsFe)
        {
            return std::vector<GalaxyBudsAnc>{
                GalaxyBudsAnc::Off,
                GalaxyBudsAnc::AmbientSound,
                GalaxyBudsAnc::NoiseReduction,
            };
        }
        else if (model == GalaxyBudsModelIds::GalaxyBuds3Pro)
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