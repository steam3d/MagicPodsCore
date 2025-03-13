#include "GalaxyBudsEarDetectionWatcher.h"

namespace MagicPodsCore{

    std::optional<GalaxyBudsEarDetectionStateArgs> GalaxyBudsEarDetectionWatcher::ExtractState(unsigned char b){
        std::optional<GalaxyBudsEarDetectionState> placementL;
        std::optional<GalaxyBudsEarDetectionState> placementR;

        if (model == GalaxyBudsModelIds::GalaxyBuds)
        {
            if (!isValidGalaxyBudsLegacyEarDetectionStateType(b))
                return std::nullopt;

            switch (static_cast<GalaxyBudsLegacyEarDetectionState>(b))
            {
                case GalaxyBudsLegacyEarDetectionState::Both:
                    placementL = GalaxyBudsEarDetectionState::Wearing;
                    placementR = GalaxyBudsEarDetectionState::Wearing;
                    break;
                case GalaxyBudsLegacyEarDetectionState::L:
                    placementL = GalaxyBudsEarDetectionState::Wearing;
                    placementR = GalaxyBudsEarDetectionState::Idle;
                    break;
                case GalaxyBudsLegacyEarDetectionState::R:
                    placementL = GalaxyBudsEarDetectionState::Idle;
                    placementR = GalaxyBudsEarDetectionState::Wearing;
                    break;
                default:
                    placementL = GalaxyBudsEarDetectionState::Idle;
                    placementR = GalaxyBudsEarDetectionState::Idle;
                    break;
            }
        }
        else if (model == GalaxyBudsModelIds::GalaxyBudsPlus ||
            model == GalaxyBudsModelIds::GalaxyBudsLive ||
            model == GalaxyBudsModelIds::GalaxyBudsPro ||
            model == GalaxyBudsModelIds::GalaxyBuds2 ||
            model == GalaxyBudsModelIds::GalaxyBuds2Pro ||
            model == GalaxyBudsModelIds::GalaxyBudsFe ||
            model == GalaxyBudsModelIds::GalaxyBuds3 ||
            model == GalaxyBudsModelIds::GalaxyBuds3Pro)
        {
            unsigned char left = (b & 0b11110000) >> 4;
            if (isValidGalaxyBudsEarDetectionStateType(left))
                placementL = static_cast<GalaxyBudsEarDetectionState>(left);

            unsigned char right = b & 0b00001111;
            if (isValidGalaxyBudsEarDetectionStateType(right))
                placementR = static_cast<GalaxyBudsEarDetectionState>(right);
        }

        if (placementL.has_value() && placementR.has_value())
            return GalaxyBudsEarDetectionStateArgs(placementL.value(), placementR.value());

        return std::nullopt;
    }

    std::optional<GalaxyBudsEarDetectionStateArgs> GalaxyBudsEarDetectionWatcher::Process_EXTENDED_STATUS_UPDATED(GalaxyBudsResponseData data)
    {
        if (data.Id != GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED)
            return std::nullopt;

        if (data.Payload.size() <= 6)
            return std::nullopt;

        return ExtractState(data.Payload[6]);
    }

    std::optional<GalaxyBudsEarDetectionStateArgs> GalaxyBudsEarDetectionWatcher::Process_STATUS_UPDATED(GalaxyBudsResponseData data)
    {
        if (data.Id != GalaxyBudsMsgIds::STATUS_UPDATED)
            return std::nullopt;

        if (data.Payload.size() <= 5)
            return std::nullopt;

        return ExtractState(data.Payload[5]);
    }

    void GalaxyBudsEarDetectionWatcher::ProcessResponse(GalaxyBudsResponseData data)
    {
        if (data.Id == GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED)
        {
            std::optional<GalaxyBudsEarDetectionStateArgs> state = Process_EXTENDED_STATUS_UPDATED(data);
            if (state.has_value())
                _earDetectionStateChanged.FireEvent(state.value());
        }
        else if (data.Id == GalaxyBudsMsgIds::STATUS_UPDATED)
        {
            std::optional<GalaxyBudsEarDetectionStateArgs> state = Process_STATUS_UPDATED(data);
            if (state.has_value())
            _earDetectionStateChanged.FireEvent(state.value());
        }
    }
}