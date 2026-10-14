// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/enums/AapAncMode.h"

namespace MagicPodsCore
{
    class AapPressAndHoldAirPodModeAncCapability : public AapCapability
    {
    private:
        // Application-only value for the UI's "Do not apply" option. It is outside
        // the byte range and is never sent to headphones.
        static constexpr int DoNotApply = -1;

        int option{DoNotApply};
        size_t clientStateChangedEventId{};

        static unsigned char ModeToFlag(AapAncMode mode);
        static bool HasAtLeastTwoModes(int value);
        unsigned char GetAvailableModes() const;
        bool IsValidOption(int value) const;

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapPressAndHoldAirPodModeAncCapability(AapDevice& device);
        ~AapPressAndHoldAirPodModeAncCapability() override;
        void SetFromJson(const nlohmann::json& json) override;

        static bool IsSupported(unsigned short model);
    };
}
