// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/enums/AapPressAndHoldAirPodMode.h"

namespace MagicPodsCore
{
    class AapPressAndHoldAirPodModeCapability : public AapCapability
    {
    private:
        // Application-only value for the UI's "Do not apply" option. It is outside
        // the byte range, is not part of AapPressAndHoldAirPodMode, and is never sent.
        static constexpr int DoNotApply = -1;

        int _rightOption{DoNotApply};
        int _leftOption{DoNotApply};
        size_t _clientStateChangedEventId{};

        static bool IsValidOption(int value);
        static std::string OptionToString(int value);
        void SendPreference();

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapPressAndHoldAirPodModeCapability(AapDevice& device);
        ~AapPressAndHoldAirPodModeCapability() override;
        void SetFromJson(const nlohmann::json& json) override;

        static bool IsSupported(unsigned short model);
    };
}
