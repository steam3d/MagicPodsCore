// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapEarDetectionStateCapability.h"
#include "AapEarDetectionHelper.h"

namespace MagicPodsCore
{
    nlohmann::json AapEarDetectionStateCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = option.Status;
        return bodyJson;
    }

    void AapEarDetectionStateCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapEarDetectionStateCapability::AapEarDetectionStateCapability(AapDevice& device) : AapCapability("earDetectionState", true, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapEarDetectionArgs state){
            DeviceEarDetectionData newOption = AapEarDetectionHelper::ToDeviceEarDetectionData(state);
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

    AapEarDetectionStateCapability::~AapEarDetectionStateCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }
}
