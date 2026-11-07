// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// the second (SMEP) link

#include "SmepChannel.h"

#include "Logger.h"
#include "sdk/sgb/smep/SmepFrame.h"

#include <algorithm>
#include <chrono>

namespace MagicPodsCore
{
    const std::vector<uint8_t>& SmepChannel::MdePrime()
    {
        // Measured: this is the MDE version the buds accept (GalaxyBudsClient's
        // SppAlternativeMessage MdeVersionFull), and the trailing 01 CB 5D is the account tag
        // the device group table echoes back as 5DCB.
        static const std::vector<uint8_t> value{0x43, 0x04, 0x03, 0x07, 0x00, 0x00, 0x0B, 0x02, 0x01, 0xCB, 0x5D};
        return value;
    }

    const std::vector<uint8_t>& SmepChannel::StateRead()
    {
        // 44 = READ_PROPERTY, then 00 02 = 0x0200 little-endian = ALL_CURRENT_STATES. This is
        // NOT a group table read - 0x0502 is push-only and never answered (17.13-M C, 18.2).
        static const std::vector<uint8_t> value{0x44, 0x00, 0x02};
        return value;
    }

    const std::vector<uint8_t>& SmepChannel::Subscribe()
    {
        // Byte-for-byte the frame the phone stack (and the patched PC client) writes: attr
        // 0x0000 = 03, then CMD_SUBSCRIBE (0x0301) = 01, both little-endian TLVs.
        static const std::vector<uint8_t> value{0x43, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01};
        return value;
    }

    SmepChannel::SmepChannel(std::string budsAddress) : _budsAddress{std::move(budsAddress)}
    {
        _client = Client::CreateRFCOMMChannel(_budsAddress, SMEP_CHANNEL);
    }

    SmepChannel::~SmepChannel()
    {
        Stop();
    }

    void SmepChannel::SetBootstrap(std::vector<uint8_t> bootstrap)
    {
        _bootstrap = std::move(bootstrap);
        if (_bootstrap.empty())
        {
            _bootstrap = MdePrime();
        }
    }

    void SmepChannel::Start()
    {
        if (_started && !_client->IsStarted())
        {
            // The underlying client hit EOF (buds powered off - the normal lid-close event)
            // while nobody called Stop(): IsStarted() already reports false through the
            // client, but a blind Start() would early-return on this stale flag forever
            // (deck log 2026-11-07 00:56: scn 29 never came back after the lid cycle).
            // Reset through Stop() first, exactly like the reconnect the device itself does.
            Stop();
        }
        if (_started)
        {
            return;
        }
        // The caller ticks this while the buds are not reachable, so keep the attempts apart;
        // a failed connect on scn 29 is normal while the buds are out of range.
        const auto now = std::chrono::steady_clock::now();
        if (_lastStartAttempt.time_since_epoch().count() != 0 && now - _lastStartAttempt < std::chrono::seconds{5})
        {
            return;
        }
        _lastStartAttempt = now;
        _started = true;

        std::vector<uint8_t> bootstrap = _bootstrap;
        _client->Start([bootstrap](Client& client) {
            // Sent right after the socket is up, before the reader can interleave: the class
            // write only counts on the first link, so it belongs to the connection, not to a
            // later decision.
            client.SendData(SmepFrame::Encode(0x01, bootstrap));
            client.SendData(SmepFrame::Encode(0x01, StateRead()));
        });

        if (!_client->IsStarted())
        {
            _started = false;
            Logger::Warn("SmepChannel: could not open scn %d on %s", SMEP_CHANNEL, _budsAddress.c_str());
            return;
        }

        {
            std::lock_guard lock{_bufferMutex};
            _openedAt = std::chrono::steady_clock::now();
            _lastRxTime = _openedAt;
        }

        _receivedDataEventId = _client->GetOnReceivedDataEvent().Subscribe(
            [this](size_t, const std::vector<uint8_t>& data) { OnReceivedData(data); });
        Logger::Info("SmepChannel: scn %d open on %s", SMEP_CHANNEL, _budsAddress.c_str());
    }

    void SmepChannel::Stop()
    {
        if (!_started)
        {
            return;
        }
        _started = false;
        _client->GetOnReceivedDataEvent().Unsubscribe(_receivedDataEventId);
        _client->Stop();
        Logger::Info("SmepChannel: scn %d closed on %s", SMEP_CHANNEL, _budsAddress.c_str());
    }

    void SmepChannel::Reopen()
    {
        Stop();
        Start();
    }

    void SmepChannel::SendBootstrap()
    {
        SendSub(_bootstrap);
    }

    bool SmepChannel::IsStarted() const
    {
        return _started && _client->IsStarted();
    }

    void SmepChannel::SendSub(const std::vector<uint8_t>& sub)
    {
        if (!IsStarted())
        {
            return;
        }
        _client->SendData(SmepFrame::Encode(0x01, sub));
    }

    void SmepChannel::SendProfileControl(uint8_t controlByte)
    {
        SendSub(std::vector<uint8_t>{0x71, controlByte});
    }

    std::chrono::steady_clock::time_point SmepChannel::GetLastRxTime() const
    {
        std::lock_guard lock{_bufferMutex};
        return _lastRxTime;
    }

    std::chrono::steady_clock::time_point SmepChannel::GetOpenedAt() const
    {
        std::lock_guard lock{_bufferMutex};
        return _openedAt;
    }

    std::vector<uint8_t> SmepChannel::GetCachedState(uint16_t attribute) const
    {
        std::lock_guard lock{_stateMutex};
        const auto entry = _stateCache.find(attribute);
        return entry == _stateCache.end() ? std::vector<uint8_t>{} : entry->second;
    }

    bool SmepChannel::EarbudsInClosedCradle() const
    {
        if (GetCachedState(ATTR_STATE_BATTERY_CRADLE) == std::vector<uint8_t>{0xFF})
        {
            return true;
        }
        return GetCachedState(ATTR_STATE_WEARING_L) == std::vector<uint8_t>{0x04};
    }

    void SmepChannel::ScanPropertyChain(const std::vector<uint8_t>& payload)
    {
        // payload[0] is the sub-id (0x44 answer / 0x45 push); behind it a chain of
        // [id:2LE][len:1][value] items runs to the end of the frame.
        size_t i = 1;
        std::string chain{};
        char buffer[16] = {0};
        while (i + 3 <= payload.size())
        {
            const uint16_t attribute = static_cast<uint16_t>(payload[i] | (payload[i + 1] << 8));
            // The firmware sometimes claims more than the frame carries (the EIR descriptor
            // lies about 256 bytes); clamp instead of rejecting the whole chain.
            const size_t length = std::min<size_t>(payload[i + 2], payload.size() - i - 3);
            std::snprintf(buffer, sizeof(buffer), " %04X", attribute);
            chain += buffer;
            if (attribute != ATTR_DEVICE_GROUP_TABLE)
            {
                std::lock_guard lock{_stateMutex};
                _stateCache[attribute] = std::vector<uint8_t>(payload.begin() + static_cast<ptrdiff_t>(i + 3),
                                                               payload.begin() + static_cast<ptrdiff_t>(i + 3 + length));
            }
            i += 3 + length;
        }
        // Both the keepalive answer (0x44) and every push (0x45) surface here: this one line is
        // the proof that CMD_SUBSCRIBE works and that the link is answering.
        Logger::Debug("SmepChannel: %s chain:%s", payload[0] == 0x45 ? "push" : "answer", chain.c_str());
    }

    void SmepChannel::OnReceivedData(const std::vector<uint8_t>& data)
    {
        std::vector<MultipointState> states{};
        {
            std::lock_guard lock{_bufferMutex};
            _lastRxTime = std::chrono::steady_clock::now();
            _rxBuffer.insert(_rxBuffer.end(), data.begin(), data.end());
            if (_rxBuffer.size() > 8192)
            {
                _rxBuffer.clear();
            }
            for (const auto& frame : SmepFrame::Decode(_rxBuffer))
            {
                if (frame.id != 0x01 || frame.payload.empty())
                {
                    continue;
                }
                if (frame.payload[0] == 0x44 || frame.payload[0] == 0x45)
                {
                    ScanPropertyChain(frame.payload);
                }
                if (auto state = ParseMultipointState(frame.payload))
                {
                    states.push_back(std::move(*state));
                }
            }
        }

        for (const auto& state : states)
        {
            _multipointStateEvent.FireEvent(state);
        }
    }
}
