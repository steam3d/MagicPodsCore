// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapStemControlStateCapability.h"

namespace MagicPodsCore
{
    nlohmann::json AapStemControlStateCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["press"] = static_cast<unsigned char>(option.Press);
        bodyJson["stem"] = static_cast<unsigned char>(option.Bud);
        return bodyJson;
    }

    void AapStemControlStateCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AapStemControlStateCapability::AapStemControlStateCapability(AapDevice& device) : AapCapability("stemControlState", true, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapStemControlState mode){
            isAvailable = true;
            option = mode;
            Logger::Info("StemControlState updated: %s, %s", AapStemControlPressToString(option.Press).c_str(), AapStemControlBudToString(option.Bud).c_str());
            _onChanged.FireEvent(*this);
        });
    }

    AapStemControlStateCapability::~AapStemControlStateCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }
}
