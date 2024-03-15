#pragma once

#include "AppleProductIds.h"
#include "BtVendorIds.h"
#include "Event.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

namespace MagicPodsCore
{

    enum class SonyAncFilter : unsigned char
    {
        Ambient = 0x00,
        Wind = 0x01,
        Anc = 0x02,
    };

    enum class SonyAncFilterAmbientVoice : unsigned char
    {
        Off = 0x00,
        On = 0x01,
    };

    enum class SonyAncSwitch : unsigned char
    {
        Off = 0x00,
        On = 0x10,
        //Save current mode to headphones
        //When user click switch mode button on headphones, the headphones will switch next mode based on saved setting.
        //Default cycle: ANC -> Ambient -> Disable.
        //If you <see cref="SonyAncSwitch.OnAndSave"/> Ambient, the next mode will be "Disable" when user click headphones button.
        //if you <see cref="SonyAncSwitch.On"/> Ambient, the next mode will be any, since we are not changing the default cycle.
        //Option also write Volume and Voice settings for ambient mode.
        //Option can override Ambient mode to Wind mode, the headphones will speak "Ambient mode", but the real mode will be wind. This is strange, but works.  
        OnAndSave = 0x11,
    };

    struct SonyAncState {
        SonyAncSwitch AncSwitch{};
        SonyAncFilter AncFilter{};
        SonyAncFilterAmbientVoice AmbientVoice{};
        unsigned char Volume{};        
    };

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
    
    // ---------------------------------------------------------------------------
    // Request
    // 3e0c00000000026602763c
    //    |Body packet          |
    // 0  1  2  3  4  5  6  7  8  9  10
    // s     p              c     cr e
    // 3e 0c 00 00 00 00 02 66 02 76 3c
    //
    // 0 byte:
    // 3e - Packet start (const)
    // 
    // 2 byte:
    // 00 - prefix
    // 01 - prefix
    //
    // 7 byte: command
    //
    // 9 byte: CRC 
    // 
    // 10 byte: 
    // 3c - Packed end (const)    
    // ---------------------------------------------------------------------------
    class SonyGetAnc : public SonyBaseCmd {
    public:
        SonyGetAnc();
        SonyAncState state{};
        void ProcessResponse(const std::vector<unsigned char> &bytes) override;
    protected:
        std::vector<unsigned char>CreatePacketBody(unsigned char prefix) const override;

    private:
        std::string DummyConvertSonyAncSwitch(SonyAncSwitch status);
        std::string DummyConvertSonyAncFilter(SonyAncFilter status);
        std::string DummyConvertSonyAncFilterAmbientVoice(SonyAncFilterAmbientVoice status);
    };
}