// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapCustomEqCapability.h"

#include "sdk/aap/setters/AapSetCustomEq.h"

namespace MagicPodsCore
{
    nlohmann::json AapCustomEqCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = {
            {"enabled", option.State == AapCustomEqState::Enabled},
            {"low", option.Low},
            {"mid", option.Mid},
            {"high", option.High}
        };
        return bodyJson;
    }

    void AapCustomEqCapability::OnReceivedData(const std::vector<unsigned char>& data)
    {
        watcher.ProcessResponse(data);
    }

    AapCustomEqCapability::AapCustomEqCapability(AapDevice& device) : AapCapability("customEq", false, device)
    {
        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapCustomEq mode)
        {
            if (!isAvailable)
            {
                isAvailable = true;
                option = mode;
                Logger::Info("%s updated: %s", name.c_str(), option.ToString().c_str());
                _onChanged.FireEvent(*this);
            }
            else if (!option.AreStatesEqual(mode))
            {
                option = mode;
                Logger::Info("%s updated: %s", name.c_str(), option.ToString().c_str());
                _onChanged.FireEvent(*this);
            }
        });
    }

    AapCustomEqCapability::~AapCustomEqCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
    }

    void AapCustomEqCapability::SetFromJson(const nlohmann::json& json)
    {
        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.is_object())
        {
            Logger::Error("Error: %s::SetFromJson expected an object", name.c_str());
            return;
        }

        if (!capability.contains("selected") || !capability.at("selected").is_object())
        {
            Logger::Error("Error: %s::SetFromJson requires a selected object", name.c_str());
            return;
        }

        const auto& selected = capability.at("selected");
        if (!selected.contains("enabled") ||
            !selected.contains("low") ||
            !selected.contains("mid") ||
            !selected.contains("high"))
        {
            Logger::Error("Error: %s::SetFromJson selected requires enabled, low, mid and high", name.c_str());
            return;
        }

        if (!selected.at("enabled").is_boolean())
        {
            Logger::Error("Error: %s::SetFromJson selected.enabled is not a boolean", name.c_str());
            return;
        }

        AapCustomEq newOption{};
        newOption.State = selected.at("enabled").get<bool>()
            ? AapCustomEqState::Enabled
            : AapCustomEqState::Disabled;

        if (!selected.at("low").is_number_integer() ||
            !selected.at("mid").is_number_integer() ||
            !selected.at("high").is_number_integer())
        {
            Logger::Error("Error: %s::SetFromJson selected.low, selected.mid and selected.high must be integers", name.c_str());
            return;
        }

        const int low = selected.at("low").get<int>();
        const int mid = selected.at("mid").get<int>();
        const int high = selected.at("high").get<int>();
        if (low < 0 || low > 100 ||
            mid < 0 || mid > 100 ||
            high < 0 || high > 100)
        {
            Logger::Error("Error: %s::SetFromJson selected.low, selected.mid and selected.high must be between 0 and 100", name.c_str());
            return;
        }

        newOption.Low = static_cast<unsigned char>(low);
        newOption.Mid = static_cast<unsigned char>(mid);
        newOption.High = static_cast<unsigned char>(high);

        SendData(AapSetCustomEq(newOption));

        // AirPods do not always report the changed value back immediately.
        if (!option.AreStatesEqual(newOption))
        {
            option = newOption;
            _onChanged.FireEvent(*this);
        }

        Logger::Debug("%s::SetFromJson set option to %s", name.c_str(), option.ToString().c_str());
    }
}
