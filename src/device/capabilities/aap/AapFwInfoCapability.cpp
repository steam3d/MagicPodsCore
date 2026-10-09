// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapFwInfoCapability.h"

namespace MagicPodsCore
{
    bool AapFwInfoCapability::AreEqual(const AapFwInfo& first, const AapFwInfo& second)
    {
        return first.Name == second.Name &&
               first.Model == second.Model &&
               first.Manufacturer == second.Manufacturer &&
               first.Serial == second.Serial &&
               first.Ver1 == second.Ver1 &&
               first.Ver2 == second.Ver2 &&
               first.HwRev == second.HwRev &&
               first.UpdaterId == second.UpdaterId &&
               first.LeftSerial == second.LeftSerial &&
               first.RightSerial == second.RightSerial &&
               first.Ver3 == second.Ver3;
    }

    nlohmann::json AapFwInfoCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["name"] = option.Name;
        bodyJson["model"] = option.Model;
        bodyJson["manufacturer"] = option.Manufacturer;
        bodyJson["serial"] = option.Serial;
        bodyJson["ver1"] = option.Ver1;
        bodyJson["ver2"] = option.Ver2;
        bodyJson["hwRev"] = option.HwRev;
        bodyJson["updaterId"] = option.UpdaterId;
        bodyJson["leftSerial"] = option.LeftSerial;
        bodyJson["rightSerial"] = option.RightSerial;
        bodyJson["ver3"] = option.Ver3;
        return bodyJson;
    }

    void AapFwInfoCapability::OnReceivedData(const std::vector<unsigned char>& data)
    {
        watcher.ProcessResponse(data);
    }

    AapFwInfoCapability::AapFwInfoCapability(AapDevice& device) : AapCapability("firmwareInfo", true, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, const AapFwInfo& mode)
        {
            if (isAvailable && AreEqual(option, mode))
                return;

            isAvailable = true;
            option = mode;
            Logger::Info("%s updated: %s", name.c_str(), option.ToString().c_str());
            _onChanged.FireEvent(*this);
        });
    }

    AapFwInfoCapability::~AapFwInfoCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }
}
