#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

namespace MagicPodsCore
{
    class SonyBaseCmd
    {
    protected:
        std::string _tag{};

    public:
        bool IsProcessed;
        explicit SonyBaseCmd(std::string tag);
        std::vector<unsigned char> Request(unsigned char prefix);
        void PrintAsHex();
        virtual void ProcessResponse(const std::vector<unsigned char> &bytes);

    protected:
        // First byte of packet for battery, ANC commands
        unsigned char startByte = 62;

        // The last byte of all packet. I do not know why
        unsigned char endByte = 60;

        // Create packet body without beginning and end. This body used to calculate CRC.
        virtual std::vector<unsigned char> CreatePacketBody(unsigned char prefix) const;

        // Add start and end bytes and calculate CRC to CreatePacketBody
        std::vector<unsigned char> CreateCompletePacket(std::vector<unsigned char> packetBody);
    };
}