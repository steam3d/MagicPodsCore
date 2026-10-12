// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"
#include "sdk/aap/enums/AapDigitalCrownMode.h"

namespace MagicPodsCore
{
    class AapDigitalCrownCapability : public AapCapability
    {
    private:
        static constexpr int DoNotApply = -1;

        int _option{DoNotApply};
        size_t _clientStateChangedEventId{};

        static std::string OptionToString(int value);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char>& data) override;

    public:
        explicit AapDigitalCrownCapability(AapDevice& device);
        ~AapDigitalCrownCapability() override;
        void SetFromJson(const nlohmann::json& json) override;

        static bool IsSupported(unsigned short model);
    };
}
