// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// the second (SMEP) link

#pragma once

#include "Event.h"
#include "client/Client.h"
#include "sdk/sgb/smep/MultipointInfo.h"

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace MagicPodsCore
{
    // Owns the second RFCOMM link to the buds (scn 29, "SMEP"), which is the only place the
    // multipoint state and the focus/class sub-messages live.
    //
    // Facts from the bench shape this class (14.19-14.22 deck runs,
    // 17.13-M/17.23/17.24 PC runs):
    //  * a 0x71 write is only obeyed on the link the buds consider first, so the link has to
    //    be long-lived rather than opened per claim;
    //  * the device group table (attr 0x0502) is push-only: a direct read never answers with
    //    it. An MDE write is the deterministic way to provoke a fresh push (<=0.3 s);
    //  * the 44 00 02 read is READ ALL_CURRENT_STATES (0x0200 little-endian), not a table
    //    read. It doubles as the link keepalive a Galaxy phone drives with its own periodic
    //    reads: hammered for whole research sessions with zero side effects, and a link that
    //    answers nothing for three intervals is half-open and needs a rebuild (17.14 A3).
    class SmepChannel
    {
    public:
        static constexpr unsigned short SMEP_CHANNEL = 29;
        // Attribute ids from the PC-side AltProperty table (SppAlternativeMessage.cs), the
        // wire encoding is little-endian inside the TLV chains.
        static constexpr uint16_t ATTR_ALL_CURRENT_STATES = 0x0200;
        static constexpr uint16_t ATTR_STATE_WEARING_L = 0x0206;
        static constexpr uint16_t ATTR_STATE_WEARING_R = 0x0207;
        static constexpr uint16_t ATTR_STATE_BATTERY_CRADLE = 0x020A;
        static constexpr uint16_t ATTR_DEVICE_GROUP_TABLE = 0x0502;
        static constexpr uint16_t ATTR_CMD_SUBSCRIBE = 0x0301;

        // MDE version write (43 ...) with the account tag the buds answer with (0xCB5D family).
        // Writing it is what makes the buds publish the device group table again (17.15), which
        // is how a class write is verified and how a stale table is refreshed deterministically.
        static const std::vector<uint8_t>& MdePrime();
        // 44 00 02 = READ_PROPERTY of ALL_CURRENT_STATES: keepalive + state snapshot.
        static const std::vector<uint8_t>& StateRead();
        // 43 + (attr 0x0000 = 03) + (attr CMD_SUBSCRIBE 0x0301 = 01): one write buys a state
        // snapshot now plus WEARING/CRADLE/battery pushes within seconds of a change, for the
        // rest of the session (per-session, no residue; coexists with the phone's own subscribes
        // and traffic gain is ~0 - measured, 17.13-M E, 17.23 U1-U3, 17.24).
        static const std::vector<uint8_t>& Subscribe();

        explicit SmepChannel(std::string budsAddress);
        ~SmepChannel();

        // bootstrap is what gets sent on every new link before the first read; the default is
        // the plain MDE prime, and the policy layer may extend it (see AutoSwitchService).
        void SetBootstrap(std::vector<uint8_t> bootstrap);
        void Start();
        void Stop();
        bool IsStarted() const;

        // Re-establishes the link (the half-open watchdog's recovery). Start()'s own rate limit
        // keeps a genuinely unreachable bud from churning.
        void Reopen();
        // Resends the bootstrap on the live link: the deterministic table refresh (17.15).
        void SendBootstrap();

        // Sends one SMEP sub-message (payload of a PROFILE_CONTROL frame) on this link.
        void SendSub(const std::vector<uint8_t>& sub);
        // 0x71 with a class/grant byte: 0x87 = claim focus (+ promote record to 07),
        // 0x81 = claim + demote back to computer, 0x02 = "report me as a phone" (no grant).
        void SendProfileControl(uint8_t controlByte);

        // Time the current link last said anything (any frame), and when it was opened: the
        // caller drives the half-open watchdog and the settle-before-subscribe delay from these.
        std::chrono::steady_clock::time_point GetLastRxTime() const;
        std::chrono::steady_clock::time_point GetOpenedAt() const;

        // Latest value seen for one state attribute (read answers and notify pushes both feed
        // the cache). Empty while nothing has been seen for that attribute.
        std::vector<uint8_t> GetCachedState(uint16_t attribute) const;
        // True when the freshest samples say the buds are stored with the lid shut: cradle
        // battery 0xFF is only readable as such with the lid closed, WEARING 4 is "in the
        // cradle after a close" (calibrated PC-side, 17.13-M F). Paging a sleeping bud with a
        // rebuild is noise; the link comes back on its own when the lid opens.
        bool EarbudsInClosedCradle() const;

        Event<MultipointState>& GetMultipointStateEvent() { return _multipointStateEvent; }

    private:
        void OnReceivedData(const std::vector<uint8_t>& data);
        // Walks the [id:2LE len:1 value] property chain behind a 44 answer / 45 push header
        // byte and feeds the state cache. Values whose claimed length overruns the frame are
        // clamped (the firmware does lie about DESCRIPTION_EIR_MANU_DATA).
        void ScanPropertyChain(const std::vector<uint8_t>& payload);

        std::string _budsAddress{};
        std::unique_ptr<Client> _client{};
        mutable std::mutex _bufferMutex{};
        std::vector<uint8_t> _rxBuffer{};
        std::vector<uint8_t> _bootstrap{MdePrime()};
        std::atomic<bool> _started{false};
        std::chrono::steady_clock::time_point _lastStartAttempt{};
        std::chrono::steady_clock::time_point _lastRxTime{};
        std::chrono::steady_clock::time_point _openedAt{};

        mutable std::mutex _stateMutex{};
        std::map<uint16_t, std::vector<uint8_t>> _stateCache{};

        Event<MultipointState> _multipointStateEvent{};
        size_t _receivedDataEventId = 0;
    };
}
