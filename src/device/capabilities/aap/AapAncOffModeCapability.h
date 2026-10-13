// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/enums/AapAncOffMode.h"

namespace MagicPodsCore
{
    class AapAncOffModeCapability : public AapCapability
    {
    private:
        // Application-only value for the UI's "Do not apply" option. It is outside
        // the byte range and is never sent to headphones.
        static constexpr int DoNotApply = -1;

        int option{DoNotApply};
        size_t clientStateChangedEventId{};

        static std::string OptionToString(int value);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapAncOffModeCapability(AapDevice& device);
        ~AapAncOffModeCapability() override;
        void SetFromJson(const nlohmann::json& json) override;

        static bool IsSupported(unsigned short model);
    };
}
