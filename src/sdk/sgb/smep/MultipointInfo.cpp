// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// device group table decoder

#include "MultipointInfo.h"

#include <cctype>
#include <cstring>

namespace MagicPodsCore
{
    // The group table arrives inside a 0x45 attribute push. Measured frames start with
    // 45 02 05 4C 0B, then a 14-byte holder block at 5..18 and two 30-byte host records at
    // 19 and 49 (AutoSwitch_Notes.md 14.22, mp/dumprec.py prints the same layout).
    namespace
    {
        constexpr size_t HOLDER_TAG_OFFSET = 19;
        constexpr size_t RECORD_OFFSETS[2] = {19, 49};

        uint16_t ReadLE16(const std::vector<uint8_t>& v, size_t at)
        {
            if (at + 1 >= v.size())
            {
                return 0;
            }
            return static_cast<uint16_t>(v[at] | (v[at + 1] << 8));
        }

        SmepHostRecord ParseSlot(const std::vector<uint8_t>& p, size_t at)
        {
            SmepHostRecord rec{};
            const size_t end = at + SmepHostRecord::SLOT_SIZE;
            if (at >= p.size())
            {
                return rec;
            }
            rec.raw = std::vector<uint8_t>(p.begin() + static_cast<ptrdiff_t>(at),
                                           p.begin() + static_cast<ptrdiff_t>(end <= p.size() ? end : p.size()));
            const auto get = [&](size_t off) -> uint8_t
            {
                return at + off < p.size() ? p[at + off] : 0;
            };
            rec.tag = static_cast<uint16_t>((get(0) << 8) | get(1));
            rec.devTypeMajor = get(2);
            rec.subDevType = get(3);
            rec.transport = get(4);
            rec.audioTransport = get(5);
            rec.byte6 = get(6);
            rec.secType = get(10);
            rec.familyStamp = static_cast<uint16_t>((get(12) << 8) | get(13));
            rec.byte14 = get(14);
            rec.account = static_cast<uint16_t>((get(15) << 8) | get(16));
            return rec;
        }
    }

    uint16_t MultipointState::TagFromAddress(const std::string& address)
    {
        // "F4:AB:5C:F2:0B:8D" -> 0x0B8D, the tail the buds use for us in the group table.
        std::vector<uint8_t> bytes{};
        uint8_t current = 0;
        int digits = 0;
        for (char c : address)
        {
            if (c == ':' || c == '-')
            {
                if (digits == 2)
                {
                    bytes.push_back(current);
                }
                current = 0;
                digits = 0;
                continue;
            }
            if (!std::isxdigit(static_cast<unsigned char>(c)))
            {
                continue;
            }
            const int value = c <= '9' ? c - '0' : (std::tolower(c) - 'a' + 10);
            current = static_cast<uint8_t>(current * 16 + value);
            ++digits;
        }
        if (digits == 2)
        {
            bytes.push_back(current);
        }
        if (bytes.size() < 2)
        {
            return 0;
        }
        return static_cast<uint16_t>((bytes[bytes.size() - 2] << 8) | bytes.back());
    }

    bool MultipointState::HasHost(uint16_t tag) const
    {
        return Host(tag) != nullptr;
    }

    const SmepHostRecord* MultipointState::Host(uint16_t tag) const
    {
        for (const auto& host : hosts)
        {
            if (host.tag == tag && tag != 0)
            {
                return &host;
            }
        }
        return nullptr;
    }

    std::vector<const SmepHostRecord*> MultipointState::Others(uint16_t localTag) const
    {
        std::vector<const SmepHostRecord*> out{};
        for (const auto& host : hosts)
        {
            // A slot of 30 zero bytes is not a host at all (tag 0), PC decoder agrees.
            if (host.tag != 0 && host.tag != localTag)
            {
                out.push_back(&host);
            }
        }
        return out;
    }

    bool MultipointState::IsHolder(uint16_t tag) const
    {
        return hosts.size() > 0 && hosts.front().tag == tag && tag != 0;
    }

    bool MultipointState::IsOutputting(uint16_t tag) const
    {
        return IsHolder(tag) && holderOutput == OUTPUT_AUDIO;
    }

    std::optional<MultipointState> ParseMultipointState(const std::vector<uint8_t>& payload)
    {
        if (payload.size() < HOLDER_TAG_OFFSET + 2 || payload[1] != 0x02 || payload[2] != 0x05)
        {
            return std::nullopt;
        }

        MultipointState state{};
        state.holderState = payload[6];
        state.holderDevType = payload[8];
        state.holderAccount = static_cast<uint16_t>((payload[9] << 8) | payload[10]);
        state.holderOutput = payload[11];

        for (size_t at : RECORD_OFFSETS)
        {
            if (at + SmepHostRecord::SLOT_SIZE > payload.size())
            {
                continue;
            }
            auto record = ParseSlot(payload, at);
            // A 30-zero-byte slot is an unused table entry, not a host; dropping it here is
            // what makes hosts.front() always mean "whoever holds the focus" even when only
            // the second slot is populated.
            if (record.tag != 0)
            {
                state.hosts.push_back(std::move(record));
            }
        }
        if (state.hosts.empty())
        {
            return std::nullopt;
        }
        return state;
    }
}
