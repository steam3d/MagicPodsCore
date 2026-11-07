// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// device group table decoder

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace MagicPodsCore
{
    // One host slot of the device group table the buds push on SMEP (attr 0x0502 is push-only;
    // an MDE write is what deterministically provokes a push, AutoSwitch_Notes.md 17.15).
    // Field meanings are shared with the PC-side GroupTableDecoder (AutoSwitch_Notes.md 18.2 #6,
    // 20.5) and are tag-relative inside the 30-byte slot:
    //   +0  tag          low two bytes of the host BD_ADDR, in the order the buds push them
    //   +2  devTypeMajor 01 computer, 02 phone, 04 media; 07 is what a host that just claimed
    //                    the focus is temporarily promoted to and drops back to 01 after ~2.5 s
    //                    (PC run, AutoSwitch_Notes.md 20.11) - not an anomaly
    //   +3  subDevType, seen 02 on every connected host surveyed
    //   +4/+5 cloud-registration flag (40 00 ours, 41 00 for the stamped phone)
    //   +6  seen 0x15 for a live host, 0x11 just after the link came up
    //   +10 secType      01 on real Samsung phones (paired through the Samsung flow), 0 on every
    //                    host we control; no host-side write path exists (AutoSwitch_Notes.md 17.13)
    //   +12 familyStamp  MCF, non-zero (b5ee) only for hosts the firmware accepts as Galaxy family
    //   +14 unknown      00/01
    //   +15 account      the 2-byte account hash written by the MDE (43 ...) sub-message (5dcb)
    //   +18.. timing: grant-validity countdown (LE u16, ~2900 = 29 s, armed at grant) and the last
    //        4 bytes = LE seconds uptime timestamp (AutoSwitch_Notes.md 17.19, 20.2). Not decoded:
    //        the policy never needs them (the release push is itself the idle event).
    // The names that are still guesses keep the wire offset in their name on purpose.
    struct SmepHostRecord
    {
        static constexpr size_t SLOT_SIZE = 30;

        uint16_t tag = 0;
        uint8_t devTypeMajor = 0;
        uint8_t subDevType = 0;
        uint8_t transport = 0;
        uint8_t audioTransport = 0;
        uint8_t byte6 = 0;
        uint8_t secType = 0;
        uint16_t familyStamp = 0;
        uint8_t byte14 = 0;
        uint16_t account = 0;
        std::vector<uint8_t> raw{};
    };

    // Everything the Auto Switch policy needs from one device group push.
    struct MultipointState
    {
        static constexpr uint8_t OUTPUT_NONE = 0x00;        // nobody is putting audio out
        static constexpr uint8_t OUTPUT_AUDIO = 0x01;       // the holder really is outputting
        static constexpr uint8_t OUTPUT_GRANTED = 0x80;     // focus granted to the holder, no audio yet

        // Header state byte (attr 0x0502 value +2). Meanings settled on the PC side with the
        // same wire (AutoSwitch_Notes.md 18.2 #6/#7, 20.5): every push names its subject in
        // the first record row -
        //   0x80 idle (nobody has the focus), 0x81 release (a grant was taken back),
        //   0x83 seen mid-transition, 0x84 semantics unknown,
        //   0x85 grant push: the first row is who was just granted the focus,
        //   0x87 streaming push: the first row is who is streaming right now, which may be a
        //      host other than the focus holder (silent continued streams, 20.5).
        static constexpr uint8_t STATE_IDLE = 0x80;
        static constexpr uint8_t STATE_RELEASE = 0x81;
        static constexpr uint8_t STATE_TRANSITION = 0x83;
        static constexpr uint8_t STATE_GRANT = 0x85;
        static constexpr uint8_t STATE_STREAM = 0x87;

        uint8_t holderState = 0;
        uint8_t holderDevType = 0;
        uint8_t holderOutput = OUTPUT_NONE;
        uint16_t holderAccount = 0;
        std::vector<SmepHostRecord> hosts{};

        // Our own two-byte tag: the low two bytes of our adapter address, in the order
        // the buds push them (F4:AB:5C:F2:0B:8D -> 0x0B8D).
        static uint16_t TagFromAddress(const std::string& address);

        bool HasHost(uint16_t tag) const;
        const SmepHostRecord* Host(uint16_t tag) const;
        std::vector<const SmepHostRecord*> Others(uint16_t localTag) const;

        bool IsHolder(uint16_t tag) const;
        // The buds list whoever holds the audio focus first, so a handover shows up as the
        // two slots swapping order (measured, AutoSwitch_Notes.md 14.22). This asks whether
        // `tag` holds focus and is really putting audio out right now.
        bool IsOutputting(uint16_t tag) const;
    };

    // payload is the SMEP frame payload of a 0x45 push: 45 02 05 4C 0B ...; returns
    // nullopt when the payload is not a device group push (the state answer we poll for).
    std::optional<MultipointState> ParseMultipointState(const std::vector<uint8_t>& payload);
}
