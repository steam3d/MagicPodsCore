// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// the policy state machine

#include "AutoSwitchService.h"

#include "AdapterTags.h"
#include "AudioStreamWatch.h"
#include "Logger.h"
#include "SteamGamePresence.h"
#include "media/MediaController.h"

#include <algorithm>
#include <cstdio>
#include <thread>

namespace MagicPodsCore
{
    namespace
    {
        using clock = std::chrono::steady_clock;

        // A group-table push older than this cannot prove anything about right now; the PC
        // client uses the same 10 s window (SmepWatchService.TableFreshness).
        constexpr auto TableFreshness = std::chrono::seconds{10};
        // Let a freshly opened link settle before subscribing (bench timing, 17.13-M E).
        constexpr auto SubscribeSettle = std::chrono::milliseconds{1500};
        // Half-open link detection: three keepalive intervals of complete silence mean the
        // socket is lying (17.14 A3). Rebuild at most once per two minutes.
        constexpr int StaleLinkMisses = 3;
        constexpr auto ReopenCooldown = std::chrono::seconds{120};
        // Claim timing (PC bench 20.9/20.11): proven contention is claimed after a short
        // debounce, unproven waits the old flash-filter window; the buds' AVRCP kill cannot
        // physically land faster than this after a stream starts, so a shorter stint is the
        // user's own second press and gets left alone.
        constexpr auto ClaimDebounceProven = std::chrono::milliseconds{300};
        constexpr auto ClaimDebounce = std::chrono::milliseconds{1500};
        constexpr auto BudsReactionFloor = std::chrono::milliseconds{800};
        // 71 87 promotes our record to 07; the demote back to "computer" 2.5 s later keeps
        // the peer able to interrupt us again (E12/E13, PC runs demote the same beat).
        constexpr auto DemoteAfterClaim = std::chrono::milliseconds{2500};
        // An MDE re-prime always provokes a table push, but keep the wire quiet.
        constexpr auto RefreshMinGap = std::chrono::seconds{2};
        // The lock's 02-class writes are only ever corrections of a fresh push, so twice a
        // second on the wire is the most they can ever demand.
        constexpr auto IdentityWriteMinGap = std::chrono::seconds{2};

        const char* PhaseName(AutoSwitchPhase phase)
        {
            switch (phase)
            {
            case AutoSwitchPhase::Disabled: return "disabled";
            case AutoSwitchPhase::WaitingForState: return "waiting-for-state";
            case AutoSwitchPhase::Holding: return "holding";
            case AutoSwitchPhase::Yielding: return "yielding";
            case AutoSwitchPhase::WaitingToReclaim: return "waiting-to-reclaim";
            case AutoSwitchPhase::Claiming: return "claiming";
            }
            return "unknown";
        }
    }

    AutoSwitchService::AutoSwitchService(std::shared_ptr<SmepChannel> channel,
                                         std::shared_ptr<AudioStreamWatch> audioWatch,
                                         std::shared_ptr<MediaController> mediaController,
                                         std::shared_ptr<SettingsService> settingsService,
                                         std::string settingsContainer,
                                         std::function<bool()> mainLinkUp)
        : _channel{std::move(channel)}, _audioWatch{std::move(audioWatch)},
          _mediaController{std::move(mediaController)}, _settingsService{std::move(settingsService)},
          _settingsContainer{std::move(settingsContainer)}, _mainLinkUp{std::move(mainLinkUp)}
    {
        // The channel's actual bootstrap at construction is the plain MDE prime, whatever
        // the settings say: the first Decide compares against THIS and installs the
        // phone-class variant when the lock is already armed (a game running at core start).
        _bootstrapCarries02 = false;
    }

    AutoSwitchService::~AutoSwitchService()
    {
        Stop();
    }

    void AutoSwitchService::Start()
    {
        if (_running)
        {
            return;
        }
        _running = true;
        _localTags = AdapterTags::LocalTags();
        std::string tagText{};
        for (const auto tag : _localTags)
        {
            char buffer[8] = {0};
            std::snprintf(buffer, sizeof(buffer), "%04X ", tag);
            tagText += buffer;
        }
        _stateEventId = _channel->GetMultipointStateEvent().Subscribe(
            [this](size_t, const MultipointState& state) { OnMultipointState(state); });
        _thread = std::thread{[this] { Run(); }};
        Logger::Info("AutoSwitch: service started, our rows in the group table: %s", tagText.c_str());
    }

    void AutoSwitchService::Stop()
    {
        if (!_running)
        {
            return;
        }
        _running = false;
        if (_thread.joinable())
        {
            _thread.join();
        }
        // Stop the SMEP link first: joining its reader closes the only window that can still
        // fire the multipoint event, so the unsubscribe below never races FireEvent.
        _channel->Stop();
        _channel->GetMultipointStateEvent().Unsubscribe(_stateEventId);
        _phase = AutoSwitchPhase::Disabled;
        Logger::Info("AutoSwitch: service stopped");
    }

    bool AutoSwitchService::IsLocalTag(uint16_t tag) const
    {
        return std::find(_localTags.begin(), _localTags.end(), tag) != _localTags.end();
    }

    // Guarded views of the fields OnMultipointState owns: Tick must never touch them
    // outside _stateMutex or the push thread and Tick race on them (UB).
    void AutoSwitchService::MarkHolderSelf()
    {
        std::lock_guard lock{_stateMutex};
        _knownHolderIsSelf = true;
    }

    std::optional<bool> AutoSwitchService::HolderIsSelf()
    {
        std::lock_guard lock{_stateMutex};
        return _knownHolderIsSelf;
    }

    bool AutoSwitchService::ReleasePushPending()
    {
        std::lock_guard lock{_stateMutex};
        return _releasePending;
    }

    AutoSwitchPhase AutoSwitchService::GetPhase() const
    {
        return _phase;
    }

    void AutoSwitchService::OnMultipointState(const MultipointState& state)
    {
        std::lock_guard lock{_stateMutex};
        _latestState = state;
        _lastTablePushAt = clock::now();
        if (!state.hosts.empty())
        {
            _knownHolderIsSelf = IsLocalTag(state.hosts.front().tag);
        }
        // The push kind is remembered so rule B can wait for the release the buds push
        // themselves (0x81, or output back to none) instead of a local timer (20.2: it arrives
        // by itself 6-10 s after the peer goes silent).
        _releasePending = state.holderState == MultipointState::STATE_RELEASE ||
                          (state.holderState == MultipointState::STATE_IDLE &&
                           state.holderOutput == MultipointState::OUTPUT_NONE);
        // The one-line push summary (Debug: the protocol trace for field work).
        std::string rows{};
        char buffer[24] = {0};
        for (const auto& host : state.hosts)
        {
            std::snprintf(buffer, sizeof(buffer), " %04X(%02X)", host.tag, host.devTypeMajor);
            rows += buffer;
        }
        Logger::Debug("AutoSwitch: group table push: state=%02X output=%02X holderDevType=%02X rows:%s%s",
                     state.holderState, state.holderOutput, state.holderDevType, rows.c_str(),
                     _releasePending ? " (release)" : "");
    }

    void AutoSwitchService::Run()
    {
        while (_running)
        {
            Tick();
            for (int i = 0; i < 10 && _running; ++i)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{100});
            }
        }
    }

    void AutoSwitchService::PauseOwnPlayers(const char* reason)
    {
        if (_wePausedPlayers)
        {
            return;
        }
        _wePausedPlayers = true;
        Logger::Info("AutoSwitch: pausing our players (%s)", reason);
        _mediaController->Pause();
    }

    void AutoSwitchService::ResumeOwnPlayers(const char* reason)
    {
        if (!_wePausedPlayers)
        {
            return;
        }
        _wePausedPlayers = false;
        Logger::Info("AutoSwitch: resuming our players (%s)", reason);
        _mediaController->Play();
    }

    void AutoSwitchService::Tick()
    {
        const auto previousPhase = _phase.load();
        Decide();
        if (previousPhase != _phase)
        {
            Logger::Info("AutoSwitch: %s -> %s", PhaseName(previousPhase), PhaseName(_phase));
        }
    }

    void AutoSwitchService::UpdateClaim(const MultipointState* state, bool fresh, bool peerPresent,
                                        bool peerHold, const AutoSwitchSettings& settings)
    {
        const auto now = clock::now();
        const bool sinkActive = _audioWatch && _audioWatch->IsOutputActive();

        if (sinkActive && !_sinkWasActive)
        {
            // Our audio just went onto the buds. If the table is stale, ask for a fresh one
            // right now: the MDE write provokes a deterministic push (17.15) and the claim
            // decision on the slow path then has real data.
            _claimEdgeAt = now;
            _claimPending = true;
            if (!fresh && now - _lastRefreshSent >= RefreshMinGap)
            {
                _lastRefreshSent = now;
                _channel->SendBootstrap();
                Logger::Info("AutoSwitch: our audio started with a stale group table, re-priming MDE "
                             "to get a fresh one before deciding on a claim");
            }
        }
        _sinkWasActive = sinkActive;

        if (_demotePending && now >= _demoteDueAt)
        {
            _demotePending = false;
            if (settings.focusLock && SteamGamePresence::IsGameRunning())
            {
                // Under the lock the post-claim demote to computer would undo our phone-class
                // identity; ManageLockIdentity rewrites the 02 correction instead (the claim's
                // 07 row only lasts a tick), and while we output, 02 holds the focus (E55).
                Logger::Info("AutoSwitch: post-claim demote skipped - the focus lock keeps our record in phone class");
            }
            else
            {
                _channel->SendProfileControl(0x81);
                Logger::Info("AutoSwitch: our record demoted back to computer, the peer may interrupt us again");
            }
        }

        if (!_claimPending)
        {
            return;
        }
        // A claim idea that sat more than a few seconds (link was down, table was missing) is
        // stale: the moment the user asked for audio has passed and the focus situation has
        // moved on. Dropping is the safe half.
        if (now - _claimEdgeAt > std::chrono::seconds{5})
        {
            _claimPending = false;
            return;
        }

        // The freshest table (fresh or not) has to say the peer holds the focus: claiming
        // while we hold it ourselves is a no-op at best and disconnects the peer at worst.
        const auto holderSelf = HolderIsSelf();
        const bool holderKnownPeer = holderSelf.has_value() && !*holderSelf;
        const bool peerHolds = fresh ? peerHold : holderKnownPeer;
        if (!peerPresent || !peerHolds)
        {
            _claimPending = false;
            return;
        }

        const bool proven = fresh && peerHold && state &&
                            (state->holderOutput == MultipointState::OUTPUT_AUDIO ||
                             state->holderOutput == MultipointState::OUTPUT_GRANTED);
        const auto debounce = proven ? ClaimDebounceProven : ClaimDebounce;
        if (now - _claimEdgeAt < debounce)
        {
            return;
        }
        _claimPending = false;

        if (!_sinkWasActive)
        {
            // The stream already stopped before we decided. If it ran shorter than the buds
            // physically need to notice it, the stop was the user's own second press and no
            // claim may replay it (M9c, 20.13); if it ran longer, the buds' own AVRCP pause
            // probably killed an unauthorised stream and the claim makes the NEXT stream
            // authorised.
            if (now - _claimEdgeAt < BudsReactionFloor)
            {
                Logger::Info("AutoSwitch: takeover dropped - the stream stopped sooner than the buds can "
                             "react, taken as a deliberate stop");
                return;
            }
            Logger::Info("AutoSwitch: our stream was killed before we could claim (the buds pause "
                         "unauthorised audio); claiming now so the source restarts into an open focus");
        }

        if (now - _lastClaim < std::chrono::seconds{settings.claimCooldownSec})
        {
            Logger::Info("AutoSwitch: takeover suppressed - still inside the %d s claim cooldown",
                         settings.claimCooldownSec);
            return;
        }

        _lastClaim = now;
        _channel->SendProfileControl(0x87);
        _demotePending = true;
        _demoteDueAt = now + DemoteAfterClaim;
        MarkHolderSelf(); // the claim just moved the focus here; do not race the push
        Logger::Info("AutoSwitch: claimed audio focus (71 87) over the persistent SMEP link");
        _phase = AutoSwitchPhase::Claiming;
    }

    void AutoSwitchService::ManageLockIdentity(const MultipointState& state, bool fresh, clock::time_point now,
                                               const AutoSwitchSettings& settings, bool lockActive)
    {
        const SmepHostRecord* ourRow = nullptr;
        for (const auto& host : state.hosts)
        {
            if (IsLocalTag(host.tag))
            {
                ourRow = &host;
                break;
            }
        }
        const bool verifiable = fresh && ourRow != nullptr; // only a fresh push tells us what our row IS

        if (lockActive)
        {
            if (!_lockIdentityActive)
            {
                _lockIdentityActive = true;
                Logger::Info("AutoSwitch: focus lock armed - our record becomes phone class (71 02); the buds "
                             "refuse the peer's handover while we output (E55). A silent Deck is NOT covered (E56).");
                if (!verifiable)
                {
                    _channel->SendBootstrap(); // provoke the table push (17.15) that lets us check the row
                }
            }
            if (verifiable && ourRow->devTypeMajor != 0x02 && now - _lastIdentityWriteAt >= IdentityWriteMinGap)
            {
                // Covers the lock arming, the claim's 07 detour and any rebuild that reset the row.
                _lastIdentityWriteAt = now;
                _channel->SendProfileControl(0x02);
                Logger::Info("AutoSwitch: focus lock - re-writing our record to phone class (71 02)");
            }
            return;
        }

        if (_lockIdentityActive)
        {
            _lockIdentityActive = false;
            if (verifiable && ourRow->devTypeMajor != 0x01 && now - _lastIdentityWriteAt >= IdentityWriteMinGap)
            {
                _lastIdentityWriteAt = now;
                _channel->SendProfileControl(0x81);
                Logger::Info("AutoSwitch: focus lock released - our record restored to computer (71 81), "
                             "the peer may take the focus again");
            }
            else if (!verifiable)
            {
                _channel->SendBootstrap(); // owe the restore: get a fresh push and the next tick writes it
            }
        }

        // NV-persistence insurance (E59 open): if a phone-class row we once wrote survives a
        // relink while the lock is off, restore cooperation.
        if (verifiable && ourRow->devTypeMajor == 0x02 &&
            now - _lastIdentityWriteAt >= IdentityWriteMinGap)
        {
            _lastIdentityWriteAt = now;
            _channel->SendProfileControl(0x81);
            Logger::Info("AutoSwitch: a phone-class record survived with the lock off - restoring computer (71 81)");
        }
    }

    void AutoSwitchService::Decide()
    {
        const AutoSwitchSettings settings = AutoSwitchSettings::Load(_settingsContainer, *_settingsService);
        if (!settings.enabled)
        {
            if (_wasEnabled)
            {
                // Mid-flight switch-off: give the cooperative identity back - a phone-class
                // record we left behind would keep the peer from ever being handed over -
                // and tear down everything we own.
                _wasEnabled = false;
                if (_wePausedPlayers)
                {
                    ResumeOwnPlayers("Auto Switch was switched off");
                }
                if (_lockIdentityActive)
                {
                    _lockIdentityActive = false;
                    _channel->SendProfileControl(0x81);
                    Logger::Info("AutoSwitch: Auto Switch switched off - our record restored to computer (71 81)");
                }
                _audioWatch->Stop();
                _channel->Stop();
            }
            _phase = AutoSwitchPhase::Disabled;
            return;
        }
        _wasEnabled = true;
        if (settings.takeover && _audioWatch)
        {
            // Idempotent: brings up the pactl-subscribe watcher on the first enabled tick.
            _audioWatch->Start();
        }

        // Focus lock state, computed here rather than in the table view: the bootstrap below
        // must be right even while the SMEP link is down and no table is in view. Records are
        // per-connection (E43), so a link that opens while the lock is armed (a lid cycle
        // mid-game) has to carry 71 02 in its first frames; the row of an already-open link is
        // handled by ManageLockIdentity.
        const bool lockActive = settings.focusLock && SteamGamePresence::IsGameRunning();
        const bool bootstrap02 = lockActive;
        if (bootstrap02 != _bootstrapCarries02)
        {
            std::vector<uint8_t> bootstrap = SmepChannel::MdePrime();
            if (bootstrap02)
            {
                bootstrap.insert(bootstrap.end(), {0x71, 0x02});
            }
            _channel->SetBootstrap(std::move(bootstrap));
            _bootstrapCarries02 = bootstrap02;
            Logger::Info("AutoSwitch: link bootstrap now %s the phone-class write", bootstrap02 ? "carries" : "drops");
        }

        // ---- link layer ----
        const bool mainLinkUp = _mainLinkUp && _mainLinkUp();
        if (!_channel->IsStarted())
        {
            // Do not page buds that last reported the closed cradle; the link comes back on
            // its own when the lid opens and the next device connect retriggers us (A4 gate,
            // 17.14: cradle battery unreadable / WEARING 4 = lid shut). But that cache is fed
            // only over the very link that is down, so a link lost while the cradle read as
            // closed would gate us out forever (deck log 2026-11-07: buds reconnected at
            // 00:34:12/00:40:19, scn 29 never returned). The gate therefore also requires the
            // main control link to be down - with the device connected the buds are provably
            // awake, and reopening scn 29 pages nothing (17.21: main link up, SMEP down ->
            // rebuild on a beat; SmepChannel::Start throttles its own attempts).
            if (!mainLinkUp && _channel->EarbudsInClosedCradle())
            {
                if (!_cradleSkipLogged)
                {
                    _cradleSkipLogged = true;
                    Logger::Info("AutoSwitch: SMEP link down and the buds last reported the closed "
                                 "cradle - waiting for the lid instead of reconnecting");
                }
                _phase = AutoSwitchPhase::WaitingForState;
                return;
            }
            _cradleSkipLogged = false;
            _channel->Start();
            _subscribed = false;
            _phase = AutoSwitchPhase::WaitingForState;
            return;
        }
        _cradleSkipLogged = false;

        const auto now = clock::now();
        const auto openedAt = _channel->GetOpenedAt();
        if (openedAt != _linkOpenedAt)
        {
            _linkOpenedAt = openedAt;
            _subscribed = false;
        }
        if (!_subscribed && now - openedAt >= SubscribeSettle)
        {
            _subscribed = true;
            _channel->SendSub(SmepChannel::Subscribe());
            Logger::Info("AutoSwitch: CMD_SUBSCRIBE written, state pushes expected from here");
        }

        const auto interval = std::chrono::milliseconds{settings.stateReadIntervalMs};
        if (now - _lastStateRead >= interval)
        {
            _lastStateRead = now;
            _channel->SendSub(SmepChannel::StateRead());
        }

        // Half-open link watchdog: an open socket that answers nothing makes Auto Switch
        // silently unavailable (17.14 A3); rebuild after three silent intervals, rate limited,
        // and not while the buds are asleep in the closed cradle.
        if (now - _channel->GetLastRxTime() >= interval * StaleLinkMisses &&
            now - _lastReopenAttempt >= ReopenCooldown)
        {
            _lastReopenAttempt = now;
            if (!mainLinkUp && _channel->EarbudsInClosedCradle())
            {
                Logger::Info("AutoSwitch: SMEP link silent, but the buds last reported the closed "
                             "cradle - skipping the rebuild until they speak again");
            }
            else
            {
                Logger::Warn("AutoSwitch: the SMEP link answered nothing for %lld s, rebuilding it",
                             static_cast<long long>(std::chrono::duration_cast<std::chrono::seconds>(
                                                        now - _channel->GetLastRxTime())
                                                        .count()));
                _channel->Reopen();
                _subscribed = false;
                std::lock_guard lock{_stateMutex};
                _latestState.reset();
                _knownHolderIsSelf.reset();
                _releasePending = false;
            }
        }

        // ---- table view ----
        std::optional<MultipointState> stateBox{};
        std::chrono::steady_clock::time_point pushAt{};
        {
            std::lock_guard lock{_stateMutex};
            stateBox = _latestState;
            pushAt = _lastTablePushAt;
        }
        const bool fresh = stateBox.has_value() && now - pushAt <= TableFreshness;

        if (!stateBox)
        {
            _phase = AutoSwitchPhase::WaitingForState;
            return;
        }
        const MultipointState& state = *stateBox;

        std::vector<const SmepHostRecord*> peers{};
        for (const auto& host : state.hosts)
        {
            if (!IsLocalTag(host.tag))
            {
                peers.push_back(&host);
            }
        }
        const bool peerPresent = !peers.empty();
        // The buds list whoever holds the focus first, so the first row says who holds it.
        const bool weHold = !state.hosts.empty() && IsLocalTag(state.hosts.front().tag);
        const bool peerHold = !state.hosts.empty() && state.hosts.front().tag != 0 && !IsLocalTag(state.hosts.front().tag);
        const bool weOutput = weHold && state.holderOutput == MultipointState::OUTPUT_AUDIO;
        const uint8_t hs = state.holderState;
        const uint8_t out = state.holderOutput;

        // Focus lock (the "lock audio focus while gaming" toggle): the identity flip IS the
        // prevention - while a Steam game runs we report as a phone (71 02), and the firmware
        // refuses to hand the audio of an outputting phone-class host to the peer (E55/E57,
        // one byte decides grabability). Runs before PEER_ABSENT so the release restore cannot
        // stall on a group that shrank to just us.
        ManageLockIdentity(state, fresh, now, settings, lockActive);

        // PEER_ABSENT: we are the only host in the table, so the focus is ours anyway. Do not
        // claim, do not keep a second host alive on purpose, and do not read "no output" as
        // the other host having finished playing - the buds drop a silent host by themselves
        // (14.20; 17.19 narrows that to pairs where neither side is a stamped phone).
        if (!peerPresent)
        {
            _peerBusyTracked = false;
            if (_wePausedPlayers && (!settings.requireGameRunning || SteamGamePresence::IsGameRunning()))
            {
                // The host we stepped aside for is gone; take our audio back under the same
                // gate rule B uses, so we never resume something on our own initiative.
                ResumeOwnPlayers("peer left the group");
            }
            _phase = weOutput ? AutoSwitchPhase::Holding : AutoSwitchPhase::WaitingForState;
            _weWereOutputting = weOutput;
            return;
        }

        // ---- takeover (Deck -> peer direction), before the yielding rules so a deliberate
        // play here wins over stepping aside ----
        if (settings.takeover)
        {
            UpdateClaim(&state, fresh, peerPresent, peerHold, settings);
        }
        const bool claimingFlow = _demotePending || _claimPending ||
                                  now - _lastClaim < std::chrono::seconds{3};

        // Fresh table only: the push kinds settled on the PC side (18.2 #6) - 0x85 names the
        // host that was just granted the focus, 0x87/output 0x01 the host that is streaming.
        // A release push (0x81) can still carry the pre-release output byte: the byte lags
        // behind the event, the state byte IS the event. Counting that as "the peer is audible"
        // makes rule A swallow the release and rule B only fire once the byte catches up or
        // the table ages out - the measured ~15 s idle-fallback resume instead of the ~1 s
        // push-driven one. Rule A's peer-busy timer must not restart from release pushes either.
        const bool peerGranted = fresh && peerHold && hs != MultipointState::STATE_RELEASE &&
                                 (out == MultipointState::OUTPUT_GRANTED || hs == MultipointState::STATE_GRANT);
        const bool peerAudible = fresh && peerHold && hs != MultipointState::STATE_RELEASE &&
                                 (out == MultipointState::OUTPUT_AUDIO || hs == MultipointState::STATE_STREAM);

        // Focus lock backstop for the window the identity flip cannot cover (E56): while OUR
        // output is stopped - game paused, menu silence - the peer can still be granted even
        // from a 02 record. Then it is the old reactive path: resume our own audio (the buds
        // hand over the second our output restarts) and claim under the cooldown; the identity
        // loop immediately rewrites the claim's 07 row back to 02.
        if (lockActive && (peerGranted || peerAudible))
        {
            _peerBusySince = now;
            _peerBusyTracked = true;
            // If we had stepped aside before the lock engaged, take our own audio back now -
            // the buds hand the focus over the second our output restarts.
            ResumeOwnPlayers("focus lock - the game owns the audio");
            if (!claimingFlow && now - _lastClaim >= std::chrono::seconds{settings.claimCooldownSec})
            {
                _lastClaim = now;
                _claimPending = false;
                MarkHolderSelf();
                _channel->SendProfileControl(0x87);
                Logger::Info("AutoSwitch: focus lock active - the peer took the focus during a game, pulled it back (71 87)");
            }
            _phase = AutoSwitchPhase::Claiming;
            _weWereOutputting = weOutput;
            return;
        }

        // Rule A: somebody else holds the focus, or has been granted it. While our own audio
        // keeps running the buds will not always complete the handover - measured on the same
        // link with nothing else changed (14.22 E55/E57) - so getting out of the way is what
        // makes the switch happen, and it works whatever class either host reports.
        if (peerGranted || peerAudible)
        {
            _peerBusySince = now;
            _peerBusyTracked = true;
            // Pausing our players is only safe while a Steam game is running: rule B resumes
            // players we paused, and rule B is gated on the same condition. Without that gate we
            // would pause a music player and never start it again. With a game running there is
            // usually no MPRIS player at all, so this stays a no-op unless something else is
            // playing on top of the game - exactly the case where our stream blocks the handover.
            // A fresh claim in flight wins: the user just asked for audio HERE.
            if (!claimingFlow && (weOutput || weHold || _weWereOutputting) && SteamGamePresence::IsGameRunning())
            {
                PauseOwnPlayers(peerGranted ? "focus granted to the other host"
                                            : "the other host is outputting");
            }
            _phase = AutoSwitchPhase::Yielding;
            _weWereOutputting = weOutput;
            return;
        }

        // Rule B: the other host went idle. The preferred trigger is the release the buds
        // push themselves (0x81, or output back to 0x00) 6-10 s after the peer stops (20.2);
        // no 0x71 claim is needed to take the focus back: the buds hand it over the same
        // second our output restarts (14.22 E56), and a claim we do not back with real audio
        // is what gets the other host disconnected (14.20 E53).
        if (_wePausedPlayers)
        {
            if (ReleasePushPending() && fresh)
            {
                ResumeOwnPlayers("the buds released the peer's grant");
            }
            else
            {
                // Fallback while that push stays missing: a local idle timer, still under the
                // rule B gate so we never resume on our own initiative.
                if (!_peerBusyTracked)
                {
                    _peerBusySince = now;
                    _peerBusyTracked = true;
                }
                const auto idleFor = std::chrono::duration_cast<std::chrono::seconds>(now - _peerBusySince).count();
                if (idleFor < settings.idleTimeoutSec || (settings.requireGameRunning && !SteamGamePresence::IsGameRunning()))
                {
                    _phase = AutoSwitchPhase::WaitingToReclaim;
                    _weWereOutputting = weOutput;
                    return;
                }
                ResumeOwnPlayers("other host idle (no release push)");
            }
        }

        if (claimingFlow)
        {
            _phase = AutoSwitchPhase::Claiming;
        }
        else
        {
            _phase = weOutput ? AutoSwitchPhase::Holding : AutoSwitchPhase::WaitingForState;
        }
        _weWereOutputting = weOutput;
    }
}
