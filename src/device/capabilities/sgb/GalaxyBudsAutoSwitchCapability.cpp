// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "GalaxyBudsAutoSwitchCapability.h"

#include "Logger.h"
#include "sdk/sgb/GalaxyBudsHelper.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"

namespace MagicPodsCore
{
    nlohmann::json GalaxyBudsAutoSwitchCapability::SelectedFromSettings() const
    {
        const AutoSwitchSettings defaults{};
        auto selected = nlohmann::json::object();
        selected["enabled"] = device.LoadSettingBool("sgb_auto_switch").value_or(defaults.enabled);
        selected["takeover"] = device.LoadSettingBool("sgb_auto_switch_takeover").value_or(defaults.takeover);
        selected["focusLock"] = device.LoadSettingBool("sgb_auto_switch_focus_lock").value_or(defaults.focusLock);
        selected["idleTimeoutSec"] = device.LoadSettingInt("sgb_auto_switch_idle_timeout_sec").value_or(defaults.idleTimeoutSec);
        return selected;
    }

    nlohmann::json GalaxyBudsAutoSwitchCapability::CreateJsonBody()
    {
        auto bodyJson = nlohmann::json::object();
        bodyJson["selected"] = SelectedFromSettings();
        return bodyJson;
    }

    // The option keys live in the device's own settings container, so every compatible
    // device carries its own choice; the policy reloads them on its next cycle.
    void GalaxyBudsAutoSwitchCapability::SetFromJson(const nlohmann::json &json)
    {
        // Models without the SMEP plane never run Auto Switch; their settings stay unwritten.
        if (!_service)
            return;

        if (!json.contains(name))
            return;

        const auto& capability = json.at(name);
        if (!capability.contains("selected") || !capability["selected"].is_object())
        {
            Logger::Error("Error: GalaxyBudsAutoSwitchCapability::SetFromJson got no selected object");
            return;
        }

        const auto& selected = capability.at("selected");
        if (selected.contains("enabled") && selected["enabled"].is_boolean())
            device.SaveSettingBool("sgb_auto_switch", selected["enabled"].get<bool>());
        if (selected.contains("takeover") && selected["takeover"].is_boolean())
            device.SaveSettingBool("sgb_auto_switch_takeover", selected["takeover"].get<bool>());
        if (selected.contains("focusLock") && selected["focusLock"].is_boolean())
            device.SaveSettingBool("sgb_auto_switch_focus_lock", selected["focusLock"].get<bool>());
        if (selected.contains("idleTimeoutSec") && selected["idleTimeoutSec"].is_number_integer())
        {
            auto idle = selected["idleTimeoutSec"].get<int>();
            if (idle < 0)
                idle = 0;
            device.SaveSettingInt("sgb_auto_switch_idle_timeout_sec", idle);
        }

        _onChanged.FireEvent(*this);
        Logger::Debug("GalaxyBudsAutoSwitchCapability::SetFromJson applied %s", selected.dump().c_str());
    }

    // Not on the GMA stream: Auto Switch talks its own protocol on the SMEP link.
    void GalaxyBudsAutoSwitchCapability::OnReceivedData(const GalaxyBudsResponseData &data)
    {
    }

    // Availability follows the buds' own connection, not the GMA control link, so the
    // base class's control-link disconnect does not tear this capability down.
    void GalaxyBudsAutoSwitchCapability::Reset()
    {
    }

    GalaxyBudsAutoSwitchCapability::GalaxyBudsAutoSwitchCapability(GalaxyBudsDevice& device,
                                                                   std::shared_ptr<SettingsService> settingsService,
                                                                   std::shared_ptr<MediaController> mediaController,
                                                                   std::shared_ptr<PulseAudioClient> audioClient,
                                                                   std::string settingsContainer)
        : GalaxyBudsCapability("autoSwitch", false, device), _settingsContainer{std::move(settingsContainer)}
    {
        // Models without the SMEP plane have no device group table to talk to: the
        // capability stays unavailable and nothing is constructed.
        if (!GalaxyBudsHelper::IsSmepMultipointModel(static_cast<GalaxyBudsModelIds>(device.GetProductId())))
            return;

        _smepChannel = std::make_shared<SmepChannel>(device.GetAddress());
        _service = std::make_unique<AutoSwitchService>(_smepChannel, audioClient, mediaController,
                                                       settingsService, _settingsContainer,
                                                       device.GetAddress(),
                                                       [this]() { return this->device.GetConnected(); });

        // The policy runs only while the buds are actually connected, and stops with them.
        connectedEventId = device.GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, bool connected)
        {
            if (connected)
            {
                if (!isAvailable)
                {
                    isAvailable = true;
                    _onChanged.FireEvent(*this);
                }
                _service->Start();
            }
            else
            {
                _service->Stop();
                if (isAvailable)
                {
                    isAvailable = false;
                    _onChanged.FireEvent(*this);
                }
            }
        });

        // The buds can already be connected when the device object is built.
        if (device.GetConnected())
        {
            isAvailable = true;
            _service->Start();
        }
    }

    GalaxyBudsAutoSwitchCapability::~GalaxyBudsAutoSwitchCapability()
    {
        device.GetConnectedPropertyChangedEvent().Unsubscribe(connectedEventId);
    }
}
