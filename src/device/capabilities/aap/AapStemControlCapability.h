// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "AapCapability.h"

namespace MagicPodsCore
{
    class AapStemControlCapability : public AapCapability
    {
    private:
        unsigned char option{};
        size_t clientStateChangedEventId{};

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AapStemControlCapability(AapDevice& device);
        ~AapStemControlCapability() override;
        static bool IsSupported(unsigned short model);
        void SetFromJson(const nlohmann::json &json) override;
    };
}
