// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsDevice.h"

#include "sdk/pbp/MaestroAddress.h"
#include "sdk/pbp/PixelBudsHelper.h"
#include "sdk/pbp/ProtoWire.h"
#include "sdk/pbp/enums/PixelBudsModelIds.h"
#include "capabilities/pbp/PixelBudsAncCapability.h"
#include "capabilities/pbp/PixelBudsBatteryCapability.h"
#include "capabilities/cmn/CmnBluetoothCodecCapability.h"

namespace MagicPodsCore
{
    void PixelBudsDevice::OnResponseDataReceived(const std::vector<unsigned char> &data)
    {
        std::vector<HdlcFrame> frames;
        {
            std::lock_guard lock{_protocolMutex};
            frames = _hdlc.Decode(data);
        }

        for (const auto &frame : frames)
        {
            if (frame.Control != Hdlc::RpcControl)
            {
                Logger::Debug("%s: unexpected hdlc control byte %d", GetName().c_str(), static_cast<int>(frame.Control));
                continue;
            }

            std::optional<PwRpcPacket> optionalPacket = PwRpcPacket::Decode(frame.Data);
            if (!optionalPacket.has_value())
            {
                Logger::Debug("%s: hdlc frame does not hold a valid rpc packet", GetName().c_str());
                continue;
            }

            OnPacketReceived(optionalPacket.value());
        }
    }

    void PixelBudsDevice::OnPacketReceived(const PwRpcPacket &packet)
    {
        Logger::Trace("%s: %s", GetName().c_str(), packet.ToString().c_str());

        bool isChannelResolved = false;

        if (packet.Type == PwRpcPacketTypes::Response &&
            packet.ServiceId == PixelBudsHelper::MaestroServiceId &&
            packet.MethodId == PixelBudsHelper::GetSoftwareInfoMethodId)
        {
            std::lock_guard lock{_protocolMutex};
            if (!_channelId.has_value())
            {
                // Only a channel the address table knows is taken. An absent field decodes as 0 and an unknown channel would leave every later rpc without an address while the probes are already stopped,
                // so such an answer is ignored and the next probe gets to resolve the channel.
                if (MaestroAddress::GetAddressForChannel(packet.ChannelId).has_value())
                {
                    _channelId = packet.ChannelId;
                    isChannelResolved = true;
                }
                else
                {
                    Logger::Warn("%s: ignoring GetSoftwareInfo answer on unknown maestro channel %u", GetName().c_str(), packet.ChannelId);
                }
            }
        }

        if (isChannelResolved)
        {
            Logger::Info("%s: maestro channel resolved: %u", GetName().c_str(), packet.ChannelId);
            SubscribeToBudsEvents();
        }

        _ResponseDataRecived.FireEvent(packet);
    }

    void PixelBudsDevice::SubscribeToBudsEvents()
    {
        // google.protobuf.Empty has no fields, so the payload of both subscriptions stays empty
        SendRpc(PixelBudsHelper::SubscribeRuntimeInfoMethodId, {});
        SendRpc(PixelBudsHelper::SubscribeToSettingsChangesMethodId, {});

        std::vector<unsigned char> readSettingMsg;
        ProtoWire::WriteVarintField(readSettingMsg, PixelBudsHelper::ReadSettingIdField, PixelBudsHelper::CurrentAncrStateSettingId);
        SendRpc(PixelBudsHelper::ReadSettingMethodId, readSettingMsg);
    }

    void PixelBudsDevice::SendRpc(uint32_t methodId, const std::vector<unsigned char> &payload)
    {
        PwRpcPacket packet{};

        {
            std::lock_guard lock{_protocolMutex};
            if (!_channelId.has_value())
            {
                Logger::Debug("%s: rpc is not sent, maestro channel is unknown", GetName().c_str());
                return;
            }

            packet.ChannelId = _channelId.value();
            packet.CallId = _nextCallId++;
        }

        packet.Type = PwRpcPacketTypes::Request;
        packet.ServiceId = PixelBudsHelper::MaestroServiceId;
        packet.MethodId = methodId;
        packet.Payload = payload;

        SendPacket(packet);
    }

    void PixelBudsDevice::SendPacket(const PwRpcPacket &packet)
    {
        std::optional<uint32_t> optionalAddress = MaestroAddress::GetAddressForChannel(packet.ChannelId);
        if (!optionalAddress.has_value())
        {
            Logger::Error("%s: no address for maestro channel %u", GetName().c_str(), packet.ChannelId);
            return;
        }

        HdlcFrame frame{};
        frame.Address = optionalAddress.value();
        frame.Control = Hdlc::RpcControl;
        frame.Data = packet.Encode();

        _client->SendData(Hdlc::Encode(frame));
    }

    // The probes only exist to find the channel, the ones left after the answer came back are of no use
    bool PixelBudsDevice::ShouldStopSendingStartData() const
    {
        std::lock_guard lock{_protocolMutex};
        return _channelId.has_value();
    }

    void PixelBudsDevice::ResetProtocolState()
    {
        std::lock_guard lock{_protocolMutex};
        _hdlc.Reset();
        _channelId.reset();
        _nextCallId = 1;
    }

    std::vector<std::vector<unsigned char>> PixelBudsDevice::CreateChannelProbes()
    {
        uint32_t channelId = MaestroAddress::GetProbeChannelId();

        PwRpcPacket packet{};
        packet.Type = PwRpcPacketTypes::Request;
        packet.ChannelId = channelId;
        packet.ServiceId = PixelBudsHelper::MaestroServiceId;
        packet.MethodId = PixelBudsHelper::GetSoftwareInfoMethodId;
        packet.CallId = PixelBudsHelper::ChannelProbeCallId;

        HdlcFrame frame{};
        frame.Address = MaestroAddress::GetAddressForChannel(channelId).value();
        frame.Control = Hdlc::RpcControl;
        frame.Data = packet.Encode();

        // Queued more than once because the start data goes out every 300ms while the answer was measured taking up to 332ms, the repeats are dropped as soon as the channel resolves
        std::vector<std::vector<unsigned char>> probes;
        for (int attempt = 0; attempt < ProbeAttempts; ++attempt)
            probes.push_back(Hdlc::Encode(frame));

        return probes;
    }

    PixelBudsDevice::PixelBudsDevice(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, unsigned short model)
        : Device(deviceInfo, audioClient, settingsService),
          _customProductId(model)
    {
        // The channel is not known in advance, GetSoftwareInfo is sent once and the answer carries the channel to use whichever one it was addressed to
        _clientStartData = CreateChannelProbes();

        _clientStateChangedId = GetClientStateChangedEvent().Subscribe([this](size_t id, ClientState state)
            {
                if (state != ClientState::Connected)
                    ResetProtocolState();
            });
    }

    PixelBudsDevice::~PixelBudsDevice()
    {
        GetClientStateChangedEvent().Unsubscribe(_clientStateChangedId);
    }

    std::shared_ptr<PixelBudsDevice> PixelBudsDevice::Create(std::shared_ptr<DBusDeviceInfo> deviceInfo, std::shared_ptr<PulseAudioClient> audioClient, std::shared_ptr<SettingsService> settingsService, unsigned short model)
    {
        // Shared from the start, so Init() can already hand a weak reference to the deferred client restart.
        auto device = std::make_shared<PixelBudsDevice>(deviceInfo, audioClient, settingsService, model);

        device->capabilities.push_back(std::make_unique<CmnBluetoothCodecCapability>(*device));
        device->capabilities.push_back(std::make_unique<PixelBudsBatteryCapability>(*device));
        device->capabilities.push_back(std::make_unique<PixelBudsAncCapability>(*device));

        device->_client = Client::CreateRFCOMM(deviceInfo->GetAddress(), PixelBudsHelper::GetServiceGuid(static_cast<PixelBudsModelIds>(model)));
        device->Init();
        return device;
    }
}
