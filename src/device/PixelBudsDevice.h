// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "Device.h"
#include "BtVendorIds.h"
#include "sdk/pbp/Hdlc.h"
#include "sdk/pbp/PwRpcPacket.h"
#include "settings/SettingsService.h"
#include <optional>
#include <cstdint>

namespace MagicPodsCore
{
    class PixelBudsDevice : public Device
    {
    private:
        // How many times the handshake probe is queued, see CreateChannelProbes.
        static constexpr int ProbeAttempts = 3;

        unsigned short _customProductId = 0;
        Event<PwRpcPacket> _ResponseDataRecived{};

        mutable std::mutex _protocolMutex{};
        Hdlc _hdlc{};
        std::optional<uint32_t> _channelId{};
        uint32_t _nextCallId = 1;
        size_t _clientStateChangedId{};

        void OnResponseDataReceived(const std::vector<unsigned char> &data) override;
        bool ShouldStopSendingStartData() const override;
        void OnPacketReceived(const PwRpcPacket &packet);
        void SubscribeToBudsEvents();
        void SendPacket(const PwRpcPacket &packet);
        void ResetProtocolState();
        static std::vector<std::vector<unsigned char>> CreateChannelProbes();

    public:
        explicit PixelBudsDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, unsigned short model);
        ~PixelBudsDevice() override;

        unsigned short GetProductId() const override
        {
            std::lock_guard lock{_propertyMutex};
            return _customProductId;
        }

        unsigned short GetVendorId() const override
        {
            return static_cast<unsigned short>(BtVendorIds::Google);
        }

        Event<PwRpcPacket> &GetResponseDataRecived()
        {
            return _ResponseDataRecived;
        }

        // The buds answer on one channel only, it is resolved by the handshake after every connect.
        void SendRpc(uint32_t methodId, const std::vector<unsigned char> &payload);

        static std::shared_ptr<PixelBudsDevice> Create(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, unsigned short model);
    };
}
