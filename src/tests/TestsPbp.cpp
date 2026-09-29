// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "TestsPbp.h"
#include "sdk/pbp/Crc32.h"
#include "sdk/pbp/Hash65599.h"
#include "sdk/pbp/Hdlc.h"
#include "sdk/pbp/MaestroAddress.h"
#include "sdk/pbp/PixelBudsHelper.h"
#include "sdk/pbp/ProtoWire.h"
#include "sdk/pbp/PwRpcPacket.h"
#include "sdk/pbp/PwVarint.h"
#include "sdk/pbp/enums/PixelBudsAncState.h"
#include "sdk/pbp/enums/PixelBudsModelIds.h"
#include "sdk/pbp/setters/PixelBudsSetAnc.h"
#include "sdk/pbp/watchers/PixelBudsAncWatcher.h"
#include "sdk/pbp/watchers/PixelBudsBatteryWatcher.h"
#include "device/capabilities/pbp/PixelBudsAncCapability.h"
#include "device/enums/DeviceAncModes.h"
#include "StringUtils.h"
#include <string>
#include <iostream>
#include <cstring>
#include <future>
#include "Logger.h"

using namespace MagicPodsCore;

// The test vectors of the framing, of the address encoding and of the id hash are taken from pbpctrl (https://github.com/qzed/pbpctrl).

namespace
{
    // Feeds one captured RuntimeInfo payload through the watcher and returns what it reported.
    std::vector<DeviceBatteryData> RunBatteryWatcher(const std::vector<unsigned char> &payload)
    {
        PwRpcPacket packet{};
        packet.Type = PwRpcPacketTypes::ServerStream;
        packet.ChannelId = 19;
        packet.ServiceId = PixelBudsHelper::MaestroServiceId;
        packet.MethodId = PixelBudsHelper::SubscribeRuntimeInfoMethodId;
        packet.Payload = payload;

        PixelBudsBatteryWatcher watcher(PixelBudsModelIds::PixelBudsPro2);

        std::promise<std::vector<DeviceBatteryData>> promise;
        std::future<std::vector<DeviceBatteryData>> future = promise.get_future();
        size_t listenerId = watcher.GetBatteryChangedEvent().Subscribe([&promise](size_t, const std::vector<DeviceBatteryData> &state)
                                                                       { promise.set_value(state); });

        watcher.ProcessResponse(packet);
        std::vector<DeviceBatteryData> state = future.get();

        watcher.GetBatteryChangedEvent().Unsubscribe(listenerId);
        return state;
    }

    bool HasBattery(const std::vector<DeviceBatteryData> &state, DeviceBatteryType type, short level, bool isCharging, DeviceBatteryStatus status)
    {
        for (const auto &battery : state)
        {
            if (battery.Type == type)
                return battery.Battery == level && battery.IsCharging == isCharging && battery.Status == status;
        }
        return false;
    }
}

// CRC-32 of "test test test"
bool TestsPbp::TestChecksum1()
{
    std::string raw = "test test test";
    std::vector<unsigned char> data(raw.begin(), raw.end());

    return Crc32::Checksum(data) == 0x235b6a02;
}

// CRC-32 of "1234321"
bool TestsPbp::TestChecksum2()
{
    std::string raw = "1234321";
    std::vector<unsigned char> data(raw.begin(), raw.end());

    return Crc32::Checksum(data) == 0xd981751c;
}

bool TestsPbp::TestVarint1()
{
    std::vector<unsigned char> expected = {6, 8, 9};

    return PwVarint::Encode(0x010203) == expected;
}

bool TestsPbp::TestVarint2()
{
    std::vector<unsigned char> raw = {6, 8, 9};

    std::optional<std::pair<uint32_t, size_t>> optdata = PwVarint::Decode(raw);
    if (!optdata.has_value())
        return false;

    return optdata.value().first == 0x010203 && optdata.value().second == 3;
}

// The largest value round trips through the longest allowed encoding.
bool TestsPbp::TestVarint3()
{
    std::vector<unsigned char> encoded = PwVarint::Encode(UINT32_MAX);
    if (encoded.size() != PwVarint::MaxEncodedLength)
        return false;

    std::optional<std::pair<uint32_t, size_t>> optdata = PwVarint::Decode(encoded);
    if (!optdata.has_value())
        return false;

    return optdata.value().first == UINT32_MAX && optdata.value().second == PwVarint::MaxEncodedLength;
}

// An unterminated run of zero bytes longer than any uint32_t encoding is rejected instead of shifting past the accumulator.
bool TestsPbp::TestVarint4()
{
    std::vector<unsigned char> raw(11, 0x00);

    return !PwVarint::Decode(raw).has_value();
}

// A terminator which shows up only after the longest allowed encoding is rejected, whatever the value in front of it.
bool TestsPbp::TestVarint5()
{
    std::vector<unsigned char> raw = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

    return !PwVarint::Decode(raw).has_value();
}

bool TestsPbp::TestHash1()
{
    return Hash65599::Hash("maestro_pw.Maestro") == 0x7ede71ea &&
           PixelBudsHelper::MaestroServiceId == 0x7ede71ea;
}

bool TestsPbp::TestHash2()
{
    return Hash65599::Hash("GetSoftwareInfo") == 0x7199fa44 &&
           PixelBudsHelper::GetSoftwareInfoMethodId == 0x7199fa44;
}

bool TestsPbp::TestAddress1()
{
    std::optional<uint32_t> optchannel = MaestroAddress::GetChannelId(MaestroPeers::MaestroA, MaestroPeers::Case);

    return optchannel.has_value() && optchannel.value() == 18;
}

bool TestsPbp::TestAddress2()
{
    std::optional<uint32_t> optaddress = MaestroAddress::GetAddressForChannel(18);
    if (!optaddress.has_value())
        return false;

    uint32_t address = optaddress.value();

    return address == ((10 << 6) | (2 << 10)) &&
           MaestroAddress::GetSource(address) == MaestroPeers::MaestroA &&
           MaestroAddress::GetTarget(address) == MaestroPeers::Case;
}

bool TestsPbp::TestEncode1()
{
    std::vector<unsigned char> expectedData = {126, 6, 8, 9, 3, 5, 6, 7, 125, 93, 125, 94, 127, 255, 230, 45, 23, 198, 126};

    HdlcFrame frame{};
    frame.Address = 0x010203;
    frame.Control = 0x03;
    frame.Data = {0x05, 0x06, 0x07, 0x7D, 0x7E, 0x7F, 0xFF};

    return Hdlc::Encode(frame) == expectedData;
}

bool TestsPbp::TestExtract1()
{
    std::vector<unsigned char> expectedPayload = {0x05, 0x06, 0x07, 0x7D, 0x7E, 0x7F, 0xFF};

    HdlcFrame frame{};
    frame.Address = 0x010203;
    frame.Control = 0x03;
    frame.Data = expectedPayload;

    Hdlc hdlc;
    std::vector<HdlcFrame> frames = hdlc.Decode(Hdlc::Encode(frame));

    if (frames.size() != 1)
        return false;

    return frames[0].Address == 0x010203 &&
           frames[0].Control == 0x03 &&
           frames[0].Data == expectedPayload;
}

// The buds answer in chunks which do not have to be aligned to the frames
bool TestsPbp::TestExtractChunks1()
{
    std::vector<unsigned char> chunk1 = {126, 6, 8, 9, 3, 5};
    std::vector<unsigned char> chunk2 = {6, 7, 125, 93, 125, 94, 127};
    std::vector<unsigned char> chunk3 = {255, 230, 45, 23, 198, 126};
    std::vector<unsigned char> expectedPayload = {0x05, 0x06, 0x07, 0x7D, 0x7E, 0x7F, 0xFF};

    Hdlc hdlc;

    if (!hdlc.Decode(chunk1).empty())
        return false;

    if (!hdlc.Decode(chunk2).empty())
        return false;

    std::vector<HdlcFrame> frames = hdlc.Decode(chunk3);
    if (frames.size() != 1)
        return false;

    return frames[0].Address == 0x010203 &&
           frames[0].Control == 0x03 &&
           frames[0].Data == expectedPayload;
}

bool TestsPbp::TestExtractChecksum1()
{
    std::vector<unsigned char> raw = {126, 6, 8, 9, 3, 5, 6, 7, 125, 93, 125, 94, 127, 255, 230, 45, 23, 198, 126};
    // Break the last byte of the payload, the checksum does not match it anymore
    raw[13] = 0xFE;

    Hdlc hdlc;
    if (!hdlc.Decode(raw).empty())
        return false;

    // A dropped frame must not break the decoding of the frames which follow it
    std::vector<unsigned char> good = {126, 6, 8, 9, 3, 5, 6, 7, 125, 93, 125, 94, 127, 255, 230, 45, 23, 198, 126};
    return hdlc.Decode(good).size() == 1;
}

bool TestsPbp::TestRpcPacket1()
{
    std::vector<unsigned char> expectedData = {8, 7, 16, 18, 29, 234, 113, 222, 126, 37, 144, 130, 30, 230, 42, 3, 1, 2, 3, 56, 9};

    PwRpcPacket packet{};
    packet.Type = PwRpcPacketTypes::ServerStream;
    packet.ChannelId = 18;
    packet.ServiceId = PixelBudsHelper::MaestroServiceId;
    packet.MethodId = PixelBudsHelper::SubscribeRuntimeInfoMethodId;
    packet.Payload = {1, 2, 3};
    packet.CallId = 9;

    return packet.Encode() == expectedData;
}

bool TestsPbp::TestRpcPacket2()
{
    std::vector<unsigned char> raw = {8, 7, 16, 18, 29, 234, 113, 222, 126, 37, 144, 130, 30, 230, 42, 3, 1, 2, 3, 56, 9};
    std::vector<unsigned char> expectedPayload = {1, 2, 3};

    std::optional<PwRpcPacket> optpacket = PwRpcPacket::Decode(raw);
    if (!optpacket.has_value())
        return false;

    PwRpcPacket packet = optpacket.value();

    return packet.Type == PwRpcPacketTypes::ServerStream &&
           packet.ChannelId == 18 &&
           packet.ServiceId == PixelBudsHelper::MaestroServiceId &&
           packet.MethodId == PixelBudsHelper::SubscribeRuntimeInfoMethodId &&
           packet.Payload == expectedPayload &&
           packet.Status == 0 &&
           packet.CallId == 9;
}

// The handshake probe, byte for byte as it was captured going out to a Pixel Buds Pro 2
bool TestsPbp::TestChannelProbe1()
{
    std::vector<unsigned char> expectedFrame = {
        0x7e, 0x00, 0x2b, 0x03, 0x10, 0x12, 0x1d, 0xea, 0x71, 0xde, 0x7d, 0x5e, 0x25, 0x44,
        0xfa, 0x99, 0x71, 0x38, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xc8, 0x5e, 0x0d, 0x12, 0x7e};

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

    return channelId == 18 && Hdlc::Encode(frame) == expectedFrame;
}

// RuntimeInfo with case 42%, left 95% charging and right 80%
bool TestsPbp::TestBattery1()
{
    std::vector<unsigned char> payload = {16, 128, 208, 149, 255, 188, 49, 50, 18, 10, 4, 8, 42, 16, 1, 18, 4, 8, 95, 16, 2, 26, 4, 8, 80, 16, 1};

    PwRpcPacket packet{};
    packet.Type = PwRpcPacketTypes::ServerStream;
    packet.ChannelId = 18;
    packet.ServiceId = PixelBudsHelper::MaestroServiceId;
    packet.MethodId = PixelBudsHelper::SubscribeRuntimeInfoMethodId;
    packet.Payload = payload;

    PixelBudsBatteryWatcher watcher(PixelBudsModelIds::PixelBudsPro2);

    std::promise<std::vector<DeviceBatteryData>> promise;
    std::future<std::vector<DeviceBatteryData>> future = promise.get_future();
    size_t listenerId = watcher.GetBatteryChangedEvent().Subscribe([&promise](size_t, const std::vector<DeviceBatteryData> &state)
                                                                   { promise.set_value(state); });

    watcher.ProcessResponse(packet);
    std::vector<DeviceBatteryData> state = future.get();

    watcher.GetBatteryChangedEvent().Unsubscribe(listenerId);

    bool isLeftPass = false, isRightPass = false, isCasePass = false;
    for (auto battery : state)
    {
        if (battery.Type == DeviceBatteryType::Left)
        {
            if (battery.Battery == 95 && battery.IsCharging == true && battery.Status == DeviceBatteryStatus::Connected)
                isLeftPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Right)
        {
            if (battery.Battery == 80 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isRightPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Case)
        {
            if (battery.Battery == 42 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isCasePass = true;
        }
    }

    return isLeftPass && isRightPass && isCasePass;
}

// RuntimeInfo captured with both buds out of the case, the case sub-message is absent
bool TestsPbp::TestBattery2()
{
    std::vector<unsigned char> payload = {24, 0, 50, 12, 18, 4, 8, 100, 16, 1, 26, 4, 8, 35, 16, 1, 58, 6, 8, 0, 16, 0, 24, 0};

    std::vector<DeviceBatteryData> state = RunBatteryWatcher(payload);

    return HasBattery(state, DeviceBatteryType::Left, 100, false, DeviceBatteryStatus::Connected) &&
           HasBattery(state, DeviceBatteryType::Right, 35, false, DeviceBatteryStatus::Connected) &&
           HasBattery(state, DeviceBatteryType::Case, 0, false, DeviceBatteryStatus::Disconnected);
}

// RuntimeInfo captured with the lid closed on the right bud, the case and the right sub-messages are absent
bool TestsPbp::TestBattery3()
{
    std::vector<unsigned char> payload = {24, 0, 50, 6, 18, 4, 8, 100, 16, 1, 58, 4, 16, 0, 24, 0};

    std::vector<DeviceBatteryData> state = RunBatteryWatcher(payload);

    return HasBattery(state, DeviceBatteryType::Left, 100, false, DeviceBatteryStatus::Connected) &&
           HasBattery(state, DeviceBatteryType::Right, 0, false, DeviceBatteryStatus::Disconnected) &&
           HasBattery(state, DeviceBatteryType::Case, 0, false, DeviceBatteryStatus::Disconnected);
}

// RuntimeInfo captured with both buds in the case, they report BatteryState 2 and the case reports 1
bool TestsPbp::TestBattery4()
{
    std::vector<unsigned char> payload = {24, 0, 50, 18, 10, 4, 8, 38, 16, 1, 18, 4, 8, 100, 16, 2, 26, 4, 8, 32, 16, 2, 58, 6, 8, 1, 16, 1, 24, 0};

    std::vector<DeviceBatteryData> state = RunBatteryWatcher(payload);

    return HasBattery(state, DeviceBatteryType::Left, 100, true, DeviceBatteryStatus::Connected) &&
           HasBattery(state, DeviceBatteryType::Right, 32, true, DeviceBatteryStatus::Connected) &&
           HasBattery(state, DeviceBatteryType::Case, 38, false, DeviceBatteryStatus::Connected);
}

// SettingsRsp holding SettingValue.current_ancr_state = ANC_STATE_ADAPTIVE
bool TestsPbp::TestAnc1()
{
    std::vector<unsigned char> payload = {34, 2, 104, 4};

    PwRpcPacket packet{};
    packet.Type = PwRpcPacketTypes::ServerStream;
    packet.ChannelId = 18;
    packet.ServiceId = PixelBudsHelper::MaestroServiceId;
    packet.MethodId = PixelBudsHelper::SubscribeToSettingsChangesMethodId;
    packet.Payload = payload;

    PixelBudsAncWatcher watcher(PixelBudsModelIds::PixelBudsPro2);

    std::promise<PixelBudsAncState> ancPromise;
    std::future<PixelBudsAncState> ancFuture = ancPromise.get_future();
    size_t listenerId = watcher.GetAncChangedEvent().Subscribe([&ancPromise](size_t, const PixelBudsAncState &ancData)
                                                               { ancPromise.set_value(ancData); });

    watcher.ProcessResponse(packet);
    PixelBudsAncState ancData = ancFuture.get();

    watcher.GetAncChangedEvent().Unsubscribe(listenerId);

    return ancData == PixelBudsAncState::Adaptive;
}

// A ReadSetting response is the answer to the request sent right after the handshake
bool TestsPbp::TestAnc2()
{
    std::vector<unsigned char> payload = {34, 2, 104, 2};

    PwRpcPacket packet{};
    packet.Type = PwRpcPacketTypes::Response;
    packet.ChannelId = 18;
    packet.ServiceId = PixelBudsHelper::MaestroServiceId;
    packet.MethodId = PixelBudsHelper::ReadSettingMethodId;
    packet.Payload = payload;

    PixelBudsAncWatcher watcher(PixelBudsModelIds::PixelBudsPro);

    std::promise<PixelBudsAncState> ancPromise;
    std::future<PixelBudsAncState> ancFuture = ancPromise.get_future();
    size_t listenerId = watcher.GetAncChangedEvent().Subscribe([&ancPromise](size_t, const PixelBudsAncState &ancData)
                                                               { ancPromise.set_value(ancData); });

    watcher.ProcessResponse(packet);
    PixelBudsAncState ancData = ancFuture.get();

    watcher.GetAncChangedEvent().Unsubscribe(listenerId);

    return ancData == PixelBudsAncState::Active;
}

// Off, transparency and noise cancellation for the Pixel Buds Pro, plus adaptive for the Pro 2
bool TestsPbp::TestAnc3()
{
    auto optionsFlagFor = [](PixelBudsModelIds model)
    {
        unsigned char optionsFlag = 0;
        for (auto state : PixelBudsAncWatcher::GetAncModesFor(model))
        {
            optionsFlag |= static_cast<unsigned char>(PixelBudsAncCapability::PixelBudsAncStateToDeviceAncModes(state));
        }
        return optionsFlag;
    };

    unsigned char expectedPro = static_cast<unsigned char>(DeviceAncModes::Off) |
                                static_cast<unsigned char>(DeviceAncModes::Transparency) |
                                static_cast<unsigned char>(DeviceAncModes::NoiseCancellation);

    unsigned char expectedPro2 = expectedPro | static_cast<unsigned char>(DeviceAncModes::Adaptive);

    return optionsFlagFor(PixelBudsModelIds::PixelBudsPro) == expectedPro &&
           optionsFlagFor(PixelBudsModelIds::PixelBudsPro2) == expectedPro2 &&
           PixelBudsAncCapability::DeviceAncModesToPixelBudsAncState(DeviceAncModes::Off) == PixelBudsAncState::Off &&
           PixelBudsAncCapability::DeviceAncModesToPixelBudsAncState(DeviceAncModes::Transparency) == PixelBudsAncState::Aware &&
           PixelBudsAncCapability::DeviceAncModesToPixelBudsAncState(DeviceAncModes::Adaptive) == PixelBudsAncState::Adaptive &&
           PixelBudsAncCapability::DeviceAncModesToPixelBudsAncState(DeviceAncModes::NoiseCancellation) == PixelBudsAncState::Active;
}

// WriteSettingMsg holding SettingValue.current_ancr_state = ANC_STATE_AWARE
bool TestsPbp::TestSetAnc1()
{
    std::vector<unsigned char> expectedPayload = {34, 2, 104, 3};

    PixelBudsSetAnc setter = PixelBudsSetAnc(PixelBudsAncState::Aware);

    return setter.MethodId == PixelBudsHelper::WriteSettingMethodId &&
           setter.Payload == expectedPayload;
}

bool TestsPbp::TestFindByClass1()
{
    std::vector<std::string> guids = {
        std::string(PixelBudsHelper::MAESTRO),
    };

    return PixelBudsHelper::IsPixelBudsDevice(guids) &&
           PixelBudsHelper::SearchModel(guids, PixelBudsHelper::PIXELBUDSPRO2CLASS, "") == PixelBudsModelIds::PixelBudsPro2;
}

bool TestsPbp::TestFindByClass2()
{
    std::vector<std::string> guids = {
        std::string(PixelBudsHelper::MAESTRO),
        std::string("0000111e-0000-1000-8000-00805f9b34fb"),
    };

    return PixelBudsHelper::SearchModel(guids, PixelBudsHelper::PIXELBUDSPROCLASS, "") == PixelBudsModelIds::PixelBudsPro;
}

bool TestsPbp::TestFindByName1()
{
    std::vector<std::string> guids = {
        std::string(PixelBudsHelper::MAESTRO),
    };

    return PixelBudsHelper::SearchModel(guids, std::nullopt, "Pixel Buds Pro 2") == PixelBudsModelIds::PixelBudsPro2 &&
           PixelBudsHelper::SearchModel(guids, std::nullopt, "Pixel Buds Pro") == PixelBudsModelIds::PixelBudsPro;
}

// Without the maestro service the device is not handled here
bool TestsPbp::TestFindByName2()
{
    std::vector<std::string> guids = {
        std::string("0000111e-0000-1000-8000-00805f9b34fb"),
    };

    return !PixelBudsHelper::IsPixelBudsDevice(guids) &&
           PixelBudsHelper::SearchModel(guids, PixelBudsHelper::PIXELBUDSPRO2CLASS, "Pixel Buds Pro 2") == PixelBudsModelIds::Unknown;
}

bool TestsPbp::TestFindByName3()
{
    std::vector<std::string> guids = {
        std::string(PixelBudsHelper::MAESTRO),
    };

    return PixelBudsHelper::SearchModel(guids, std::nullopt, "Pixel Buds A-Series") == PixelBudsModelIds::Unknown &&
           PixelBudsHelper::GetServiceGuid(PixelBudsModelIds::Unknown) == "" &&
           PixelBudsHelper::GetServiceGuid(PixelBudsModelIds::PixelBudsPro2) == PixelBudsHelper::MAESTRO;
}

void TestsPbp::Test(const char *name, bool b)
{

    int spaceCount = 50 - std::strlen(name);
    std::string space = "";
    for (int i = 0; i < spaceCount; i++)
    {
        space += " ";
    }

    if (b)
    {
        Logger::Debug("%s%s: PASS", name, space.c_str());
    }
    else
    {
        Logger::Debug("%s%s: FAIL", name, space.c_str());
    }
}

MagicPodsCore::TestsPbp::TestsPbp()
{
    Test("CRC32.Checksum1", TestChecksum1());
    Test("CRC32.Checksum2", TestChecksum2());
    Test("PwVarint.TestVarint1", TestVarint1());
    Test("PwVarint.TestVarint2", TestVarint2());
    Test("PwVarint.TestVarint3", TestVarint3());
    Test("PwVarint.TestVarint4", TestVarint4());
    Test("PwVarint.TestVarint5", TestVarint5());
    Test("Hash65599.TestHash1", TestHash1());
    Test("Hash65599.TestHash2", TestHash2());
    Test("MaestroAddress.TestAddress1", TestAddress1());
    Test("MaestroAddress.TestAddress2", TestAddress2());
    Test("Hdlc.TestEncode1", TestEncode1());
    Test("Hdlc.TestExtract1", TestExtract1());
    Test("Hdlc.TestExtractChunks1", TestExtractChunks1());
    Test("Hdlc.TestExtractChecksum1", TestExtractChecksum1());
    Test("PwRpcPacket.TestRpcPacket1", TestRpcPacket1());
    Test("PwRpcPacket.TestRpcPacket2", TestRpcPacket2());
    Test("PixelBudsDevice.TestChannelProbe1", TestChannelProbe1());
    Test("PixelBudsBatteryWatcher.TestBattery1", TestBattery1());
    Test("PixelBudsBatteryWatcher.TestBattery2", TestBattery2());
    Test("PixelBudsBatteryWatcher.TestBattery3", TestBattery3());
    Test("PixelBudsBatteryWatcher.TestBattery4", TestBattery4());
    Test("PixelBudsAncWatcher.TestAnc1", TestAnc1());
    Test("PixelBudsAncWatcher.TestAnc2", TestAnc2());
    Test("PixelBudsAncCapability.TestAnc3", TestAnc3());
    Test("PixelBudsSetAnc.TestSetAnc1", TestSetAnc1());
    Test("PixelBudsHelper.FindByClass1", TestFindByClass1());
    Test("PixelBudsHelper.FindByClass2", TestFindByClass2());
    Test("PixelBudsHelper.FindByName1", TestFindByName1());
    Test("PixelBudsHelper.FindByName2", TestFindByName2());
    Test("PixelBudsHelper.FindByName3", TestFindByName3());
}
