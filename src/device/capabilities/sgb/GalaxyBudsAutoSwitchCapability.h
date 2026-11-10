// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "GalaxyBudsCapability.h"
#include "multipoint/AutoSwitchService.h"
#include "multipoint/SmepChannel.h"

#include <memory>

namespace MagicPodsCore
{
    // Auto Switch: moves the audio focus between this host and the other device paired
    // with the buds. The firmware keeps the authority on who holds the focus in its
    // device group table, reached over a second RFCOMM link (scn 29); only the Galaxy
    // Buds generation from the Buds2 Pro up carries that plane, so on every other model
    // the capability is never offered and the second link is never opened.
    class GalaxyBudsAutoSwitchCapability : public GalaxyBudsCapability
    {
    private:
        std::shared_ptr<SmepChannel> _smepChannel{};
        std::unique_ptr<AutoSwitchService> _service{};
        std::string _settingsContainer{};
        size_t connectedEventId = 0;
        nlohmann::json SelectedFromSettings() const;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const GalaxyBudsResponseData &data) override;
        void Reset() override;

    public:
        explicit GalaxyBudsAutoSwitchCapability(GalaxyBudsDevice& device,
                                                std::shared_ptr<SettingsService> settingsService,
                                                std::shared_ptr<MediaController> mediaController,
                                                std::shared_ptr<PulseAudioClient> audioClient,
                                                std::string settingsContainer);
        ~GalaxyBudsAutoSwitchCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}
