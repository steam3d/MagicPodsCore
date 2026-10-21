// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsEarDetectionStateCapability.h"
#include "GalaxyBudsEarDetectionHelper.h"

namespace MagicPodsCore
{
    nlohmann::json GalaxyBudsEarDetectionStateCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option.Status;
        return bodyJson;
    }

    void GalaxyBudsEarDetectionStateCapability::OnReceivedData(const GalaxyBudsResponseData &data)
    {
        watcher.ProcessResponse(data);
    }

    GalaxyBudsEarDetectionStateCapability::GalaxyBudsEarDetectionStateCapability(GalaxyBudsDevice& device) : GalaxyBudsCapability("earDetectionState", true, device),
                                                                                                              watcher(GalaxyBudsEarDetectionWatcher(static_cast<GalaxyBudsModelIds>(device.GetProductId())))
    {
        watcherEventId = watcher.GetEarDetectionStateChangedEvent().Subscribe([this](size_t id, GalaxyBudsEarDetectionStateArgs state){
            DeviceEarDetectionData newOption = GalaxyBudsEarDetectionHelper::ToDeviceEarDetectionData(state);
            if (!isAvailable){
                isAvailable = true;
                option = newOption;
                Logger::Info("EarDetectionState updated: %s", option.ToString().c_str());
                _onChanged.FireEvent(*this);
            }
            else if (!option.AreStatesEqual(newOption)){
                option = newOption;
                Logger::Info("EarDetectionState updated: %s", option.ToString().c_str());
                _onChanged.FireEvent(*this);
            }
        });
    }

    GalaxyBudsEarDetectionStateCapability::~GalaxyBudsEarDetectionStateCapability()
    {
        watcher.GetEarDetectionStateChangedEvent().Unsubscribe(watcherEventId);
    }
}
