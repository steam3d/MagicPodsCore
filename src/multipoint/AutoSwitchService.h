// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// the policy state machine

#pragma once

#include "AutoSwitchSettings.h"
#include "SmepChannel.h"
#include "sdk/sgb/smep/MultipointInfo.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

namespace MagicPodsCore
{
    class MediaController;
    class AudioStreamWatch;

    // What the policy is doing right now (exposed for diagnostics/tests).
    enum class AutoSwitchPhase
    {
        Disabled,          // the feature is off
        WaitingForState,   // link is not up, or no device group table seen yet
        Holding,           // we hold the output, nothing to do
        Yielding,          // we let another host take over and paused our own players
        WaitingToReclaim,  // the other host is silent, we are waiting to resume our players
        Claiming,          // our own audio went onto the buds while the peer held focus
    };

    // Auto Switch (behaviour measured on paired PC and Deck hosts, 20.2/20.9/20.11):
    //  * rule A - a fresh group-table push says the other host was granted the focus (0x85)
    //    or is streaming (0x87 / output 0x01): pause our own players so the buds can finish
    //    the handover.
    //  * rule B - the release the buds push themselves 6-10 s after the peer goes silent
    //    (state 0x81, or output back to 0x00) is the idle event; it resumes our players. The
    //    idle_timeout_sec timer is only the fallback while that push stays missing. No 0x71
    //    claim is needed to take the focus back: the buds hand it over the same second our
    //    output restarts (deck bench 14.22 E56).
    //  * takeover - when OUR audio goes onto the buds (PipeWire sink -> RUNNING) while the
    //    peer holds focus, claim it (71 87) within ~0.5 s: while the peer holds the focus the
    //    buds kill unauthorised streams with an AVRCP pause 0.8-1.2 s after the stream starts
    //    (PC bench 20.9), and the demote (71 81) after 2.5 s puts our record back to "computer"
    //    so the peer can still interrupt us afterwards (E13/E16).
    //  * PEER_ABSENT - the other host is not in the device group table at all: no claiming,
    //    no keepalive pressure; the buds drop silent hosts by themselves (14.20, refined by
    //    17.19: stamped-phone + PC pairs are never dropped).
    //  * link layer - the 44 00 02 read is the keepalive (45 s); three silent intervals mean
    //    a half-open link and it gets rebuilt unless the buds last reported the closed cradle
    //    (paging a sleeping bud is noise, PC bench 17.13-M F). A stale table is refreshed by
    //    re-sending the MDE prime, the deterministic table trigger (17.15).
    class AutoSwitchService
    {
    public:
        // mainLinkUp answers "is the device's own control link connected" - the buds being
        // reachable is stronger evidence than our (link-fed, therefore stale when the link is
        // down) closed-cradle cache, and keeps the A4 gate from deadlocking (17.21).
        AutoSwitchService(std::shared_ptr<SmepChannel> channel,
                          std::shared_ptr<AudioStreamWatch> audioWatch,
                          std::shared_ptr<MediaController> mediaController,
                          std::shared_ptr<SettingsService> settingsService,
                          std::function<bool()> mainLinkUp);
        ~AutoSwitchService();

        void Start();
        void Stop();

        AutoSwitchPhase GetPhase() const;

    private:
        void Run();
        void Tick();
        // The decisions themselves, without the phase-change logging Tick adds.
        void Decide();
        // Focus lock identity management: assert 02 while armed, restore 01 on release, and
        // re-assert after claims or link rebuilds moved the row (E43/E59 insurance).
        void ManageLockIdentity(const MultipointState& state, bool fresh, std::chrono::steady_clock::time_point now,
                                const AutoSwitchSettings& settings, bool lockActive);
        bool IsLocalTag(uint16_t tag) const;
    // Guarded views of push-owned state (see OnMultipointState): Tick touches the fields
    // below only through these, or it races the SMEP reader thread.
    void MarkHolderSelf();
    std::optional<bool> HolderIsSelf();
    bool ReleasePushPending();
        void OnMultipointState(const MultipointState& state);
        void PauseOwnPlayers(const char* reason);
        void ResumeOwnPlayers(const char* reason);
        // The takeover side: sink-RUNNING edge bookkeeping and the claim/demote writes.
        void UpdateClaim(const MultipointState* state, bool fresh, bool peerPresent, bool peerHold,
                        const AutoSwitchSettings& settings);

        std::shared_ptr<SmepChannel> _channel{};
        std::shared_ptr<AudioStreamWatch> _audioWatch{};
        std::shared_ptr<MediaController> _mediaController{};
        std::shared_ptr<SettingsService> _settingsService{};
        std::function<bool()> _mainLinkUp{};
        // Our own rows in the group table: the tails of this machine's Bluetooth adapters.
        // Everything else in the table is the other host.
        std::vector<uint16_t> _localTags{};
        std::thread _thread{};
        std::atomic<bool> _running{false};
        std::atomic<AutoSwitchPhase> _phase{AutoSwitchPhase::Disabled};

        std::mutex _stateMutex{};
        std::optional<MultipointState> _latestState{};
        std::chrono::steady_clock::time_point _lastTablePushAt{};
        // The freshest table we ever saw said who holds the focus (used when the table itself
        // is too old to act on but the question is still "peer or us").
        std::optional<bool> _knownHolderIsSelf{};
        // True while the latest push was a release (0x81 / output back to none): that push is
        // the idle event rule B waits for.
        bool _releasePending = false;
        size_t _stateEventId = 0;

        // Focus lock v2 (fake-phone identity): while armed we keep our record at devType 02.
        // The decisive A/B (14.22 E55/E57, run on this very Deck): a host that reports itself
        // as a phone AND is currently outputting cannot be handed over to by the firmware -
        // the peer's grab is refused outright, not pulled back afterwards. 01 can be grabbed,
        // 02 cannot; that one byte is the whole difference. 02 carries no grant bit, so the
        // 40 s silent-holder eviction (14.20) never arms either.
        bool _lockIdentityActive = false;
        bool _bootstrapCarries02 = false;
        std::chrono::steady_clock::time_point _lastIdentityWriteAt{};
        // Only set when this service paused the players, so a player the user paused by hand
        // is never resumed by us.
        bool _wePausedPlayers = false;
    bool _wasEnabled = true;
        // Was our own output going last tick - a handover that starts without us noticing
        // still has to make us step aside.
        bool _weWereOutputting = false;

        std::chrono::steady_clock::time_point _lastStateRead{};
        std::chrono::steady_clock::time_point _lastReopenAttempt{};
        std::chrono::steady_clock::time_point _linkOpenedAt{};
        bool _subscribed = false;
        bool _cradleSkipLogged = false;

        std::chrono::steady_clock::time_point _peerBusySince{};
        bool _peerBusyTracked = false;

        // Takeover bookkeeping.
        bool _sinkWasActive = false;
        bool _claimPending = false;
        std::chrono::steady_clock::time_point _claimEdgeAt{};
        std::chrono::steady_clock::time_point _lastClaim{};
        std::chrono::steady_clock::time_point _lastRefreshSent{};
        bool _demotePending = false;
        std::chrono::steady_clock::time_point _demoteDueAt{};
    };
}
