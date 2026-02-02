// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include "AapCapability.h"
#include "sdk/aap/watchers/AapPrivateKeysWatcher.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "device/structs/DeviceBatteryData.h"
#include <string>
#include <optional>
#include <sstream>
#include <iomanip>
#include <vector>
#include <mutex>

namespace MagicPodsCore
{
    struct bleData
    {
        std::vector<DeviceBatteryData> batteryData;
        bool animation;
        uint8_t color;
        AapModelIds model;

        std::string ToString() const
        {
            std::ostringstream os;
            
            for (const auto& b : batteryData){
                os << DeviceBatteryTypeToString(b.Type) << ": " << DeviceBatteryStatusToString(b.Status) << ": " << static_cast<int>(b.Battery) << " " << (b.IsCharging ? "*" : " ") << "\n";
            }            
            
            os << "Color: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color) << "\n";
            os << "Model: " << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(model) << "\n";            
            os << "Animation: " << (animation ? "Show" : "Hide") << "\n";            
            return os.str();
        }

        bool operator==(const bleData &other) const
        {
            return batteryData == other.batteryData && animation == other.animation && color == other.color && model == other.model;
        }
/*
        
        bool operator==(const bleData &other) const
        {
            if (batteryData.size() != other.batteryData.size())
            {
                std::cerr << "size != " << batteryData.size() << " vs " << other.batteryData.size() << "\n";
                return false;
            }
            for (size_t i = 0; i < batteryData.size(); ++i)
            {
                const auto &a = batteryData[i];
                const auto &b = other.batteryData[i];
                if (!(a == b))
                {
                    std::cerr << "Battery[" << i << "] !=\n";
                    std::cerr << "a: " << int(a.Type) << " " << a.IsCharging << " " << int(a.Status) << " " << a.Battery << "\n";
                    std::cerr << "b: " << int(b.Type) << " " << b.IsCharging << " " << int(b.Status) << " " << b.Battery << "\n";
                    return false;
                }
            }
            if (animation != other.animation)
            {
                std::cerr << "animation != " << animation << " vs " << other.animation << "\n";
                return false;
            }
            if (color != other.color)
            {
                std::cerr << "color != " << int(color) << " vs " << int(other.color) << "\n";
                return false;
            }
            if (model != other.model)
            {
                std::cerr << "model != " << int(model) << " vs " << int(other.model) << "\n";
                return false;
            }
            return true;
        }
*/            
    };
    class AppAnimationCapability : public AapCapability
    {
    private:
        bool option;
        AapPrivateKeysWatcher watcher{};
        size_t watcherEventId;
        size_t connectionEventId;
        size_t leEventId;

        std::optional<bleData> cachedData;
        std::string mac {};
        mutable std::mutex locker;

        std::string irk; 
        std::string enc; 
        
        void FireAnimation(const bleData &bleData);
        static bool IsAnimationSupport(unsigned short productId);
        static uint8_t ClampBattery(uint8_t value);
        static uint8_t ClampNativeBattery(uint8_t value);

    protected:
        nlohmann::json CreateJsonBody() override;
        void OnReceivedData(const std::vector<unsigned char> &data) override;

    public:
        explicit AppAnimationCapability(AapDevice &device);
        ~AppAnimationCapability() override;
        void SetFromJson(const nlohmann::json &json) override {};

        static std::optional<bleData> ParseBle(const std::vector<uint8_t> &buffer, unsigned short model, const std::string &enc = {});
    };
}