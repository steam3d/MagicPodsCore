// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "Hdlc.h"
#include "Crc32.h"
#include "PwVarint.h"
#include "Logger.h"

namespace MagicPodsCore
{
    std::vector<unsigned char> Hdlc::Encode(const HdlcFrame &frame)
    {
        std::vector<unsigned char> payload = PwVarint::Encode(frame.Address);
        payload.push_back(frame.Control);
        payload.insert(payload.end(), std::begin(frame.Data), std::end(frame.Data));

        uint32_t crc32 = Crc32::Checksum(payload);
        for (int i = 0; i < 4; i++)
        {
            payload.push_back(static_cast<unsigned char>((crc32 >> (i * 8)) & 0xFF));
        }

        std::vector<unsigned char> writer;
        writer.reserve(payload.size() + 8);

        writer.push_back(frameFlag);
        for (unsigned char byte : payload)
        {
            if (byte == frameFlag || byte == escapeFlag)
            {
                writer.push_back(escapeFlag);
                writer.push_back(static_cast<unsigned char>(byte ^ escapeMask));
            }
            else
            {
                writer.push_back(byte);
            }
        }
        writer.push_back(frameFlag);

        return writer;
    }

    std::optional<HdlcFrame> Hdlc::ExtractBuffered()
    {
        if (isCorrupt)
        {
            Logger::Debug("Hdlc: frame is malformed, dropped");
            return std::nullopt;
        }

        if (buffer.size() < minFrameSize)
        {
            Logger::Debug("Hdlc: frame is too small, dropped");
            return std::nullopt;
        }

        uint32_t expectedCrc32 = 0;
        for (int i = 0; i < 4; i++)
        {
            expectedCrc32 |= static_cast<uint32_t>(buffer[buffer.size() - 4 + i]) << (i * 8);
        }

        if (Crc32::Checksum(buffer.data(), buffer.size() - 4) != expectedCrc32)
        {
            Logger::Debug("Hdlc: invalid checksum, frame dropped");
            return std::nullopt;
        }

        std::optional<std::pair<uint32_t, size_t>> optionalAddress = PwVarint::Decode(buffer);
        if (!optionalAddress.has_value())
        {
            Logger::Debug("Hdlc: invalid address, frame dropped");
            return std::nullopt;
        }

        const auto &[address, addressSize] = optionalAddress.value();

        // address + control + CRC-32
        if (buffer.size() < addressSize + 5)
        {
            Logger::Debug("Hdlc: frame is too small, dropped");
            return std::nullopt;
        }

        HdlcFrame frame{};
        frame.Address = address;
        frame.Control = buffer[addressSize];
        frame.Data.assign(std::begin(buffer) + addressSize + 1, std::end(buffer) - 4);

        return frame;
    }

    std::vector<HdlcFrame> Hdlc::Decode(const std::vector<unsigned char> &chunk)
    {
        std::vector<HdlcFrame> frames;

        for (unsigned char byte : chunk)
        {
            if (byte == frameFlag)
            {
                // The closing flag of a frame may be the opening flag of the next one.
                if (isInFrame && !buffer.empty())
                {
                    if (!isEscaped)
                    {
                        if (std::optional<HdlcFrame> optionalFrame = ExtractBuffered(); optionalFrame.has_value())
                            frames.push_back(std::move(optionalFrame.value()));
                    }
                    else
                    {
                        Logger::Debug("Hdlc: unexpected end of frame, frame dropped");
                    }
                }

                buffer.clear();
                isInFrame = true;
                isEscaped = false;
                isCorrupt = false;
                continue;
            }

            if (!isInFrame)
                continue;

            if (byte == escapeFlag)
            {
                // Two escape flags in a row are not a valid escape sequence.
                if (isEscaped)
                    isCorrupt = true;

                isEscaped = !isEscaped;
                continue;
            }

            if (buffer.size() < maxFrameSize)
            {
                buffer.push_back(isEscaped ? static_cast<unsigned char>(byte ^ escapeMask) : byte);
            }
            else
            {
                isCorrupt = true;
            }
            isEscaped = false;
        }

        return frames;
    }

    void Hdlc::Reset()
    {
        buffer.clear();
        isInFrame = false;
        isEscaped = false;
        isCorrupt = false;
    }
}
