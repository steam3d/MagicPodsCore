// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "../Capability.h"
#include "device/Device.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace MagicPodsCore
{
    class CmnBluetoothCodecCapability : public Capability
    {
    private:        
        size_t onConnectedPropertyChangedId;                        
        size_t onAudioCardPropertyChangedId;
        CardInfo info{};                       
        void UpdateCodecInfo();
        void UpdateCardInfo(const CardInfo& newinfo);
        bool IsValidSelected(const std::string& selected);
    protected:
        Device& device;
        nlohmann::json CreateJsonBody() override;
        void Reset() override;

    public:
        explicit CmnBluetoothCodecCapability(Device& device);
        ~CmnBluetoothCodecCapability() override;
        void SetFromJson(const nlohmann::json &json) override;
    };
}
