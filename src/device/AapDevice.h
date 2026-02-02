// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "Device.h"
#include "sdk/aap/setters/AapRequest.h"
#include "ble_ads/DBusBasedBleAdvertisingService.h"

namespace MagicPodsCore
{
    class AapDevice : public Device
    {
    private:
        std::shared_ptr<DBusBasedBleAdvertisingService> _bleService{};
        size_t _getOnAdReceivedEventId = 0;
        Event<const std::vector<unsigned char>> _onResponseDataRecived{};
        Event<const BleAdertisingData> _onLeDataReceived{};        
        Event<const nlohmann::json> _onAnimationTriggered{};
        void OnResponseDataReceived(const std::vector<unsigned char> &data) override;

    public:
        explicit AapDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<DBusBasedBleAdvertisingService> bleService);
        ~AapDevice() override;
        Event<const std::vector<unsigned char>> &GetResponseDataRecived()
        {
            return _onResponseDataRecived;
        }

        Event<const BleAdertisingData> &GetLeDataReceived()
        {
            return _onLeDataReceived;
        }

        Event<const nlohmann::json> &GetAnimationTriggered()
        {
            return _onAnimationTriggered;
        }

        void SendData(const AapRequest &setter);
        void FireAnimation(const nlohmann::json &json);
        static std::unique_ptr<AapDevice> Create(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<DBusBasedBleAdvertisingService> bleService);
    };
}