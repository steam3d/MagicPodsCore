// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "device/capabilities/Capability.h"

namespace MagicPodsCore
{
    class Device;

    class CmnSendHexCapability : public Capability
    {
    private:
        Device& _device;

        static bool IsValidHex(const std::string& value);

    protected:
        nlohmann::json CreateJsonBody() override { return {}; }

    public:
        explicit CmnSendHexCapability(Device& device);
        void SetFromJson(const nlohmann::json& json) override;
    };
}
