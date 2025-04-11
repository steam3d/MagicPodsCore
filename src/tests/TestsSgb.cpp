#include "TestsSgb.h"
#include "sdk/sgb/Crc16.h"
#include "sdk/sgb/enums/GalaxyBudsMsgIds.h"
#include "sdk/sgb/GalaxyBudsHelper.h"
#include "sdk/sgb/enums/GalaxyBudsModelIds.h"
#include "sdk/sgb/enums/GalaxyBudsColoredModelIds.h"
#include "sdk/sgb/GalaxyBudsPacket.h"
#include "sdk/sgb/enums/GalaxyBudsAnc.h"
#include "sdk/sgb/watchers/GalaxyBudsAncWatcher.h"
#include "sdk/sgb/watchers/GalaxyBudsEarDetectionWatcher.h"
#include "sdk/sgb/watchers/GalaxyBudsBatteryWatcher.h"
#include "sdk/sgb/setters/GalaxyBudsSetAnc.h"
#include "StringUtils.h"
#include <string>
#include <iostream>
#include <cstring>
#include <future>
#include "Logger.h"

using namespace MagicPodsCore;

// 6102086464010111000000ff22000054015401070004dd0004041000010000110200000000000000000000000000000100000200010000ff010100540f
bool TestsSgb::TestChecksum1()
{
    std::vector<unsigned char> raw = {97, 2, 8, 100, 100, 1, 1, 17, 0, 0, 0, 255, 34, 0, 0, 84, 1, 84, 1, 7, 0, 4, 221, 0, 4, 4, 16, 0, 1, 0, 0, 17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 255, 1, 1, 0, 84, 15};

    MagicPodsCore::Crc16 crc16;
    uint16_t crc = crc16.Checksum(raw);

    return crc == 0;
}

// fd04007803 93 b1 dd
bool TestsSgb::TestChecksum2()
{
    std::vector<unsigned char> raw = {3};

    MagicPodsCore::Crc16 crc16;
    std::vector<unsigned char> checksum = crc16.Checksum(MagicPodsCore::GalaxyBudsMsgIds::NOISE_CONTROLS, raw);
    return checksum[0] == 147 && checksum[1] == 177;
}

// Finder
bool TestsSgb::TestFindByGuid1()
{
    std::vector<std::string> guids = {
        "d908aab5-7a90-4cbe-8641-86a553db0147",
        std::string(GalaxyBudsHelper::GALAXYBUDSNEW),
    };
    GalaxyBudsHelper helper;
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds2Pro &&
           pairModelColor.second == StringUtils::int16ToHex(static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds2ProWhite));
}

bool TestsSgb::TestFindByGuid2()
{
    std::vector<std::string> guids = {
        "d908aab5-7a90-4cbe-8641-86a553db0154",
        std::string(GalaxyBudsHelper::GALAXYBUDSNEW),
    };
    GalaxyBudsHelper helper;
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds3Pro &&
           pairModelColor.second == StringUtils::int16ToHex(static_cast<int16_t>(GalaxyBudsColoredModelIds::GalaxyBuds3ProSilver));
}

bool TestsSgb::TestFindByGuid3()
{
    std::vector<std::string> guids = {
        "d908aab5-7a90-4cbe-8641-86a553db-X",
        std::string(GalaxyBudsHelper::GALAXYBUDSNEW),
        std::string(GalaxyBudsHelper::HANDSFREESERVICECLASS),
        std::string(GalaxyBudsHelper::LEAUDIO),
    };
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds2Pro &&
           pairModelColor.second == "";
}

bool TestsSgb::TestFindByGuid4()
{
    std::vector<std::string> guids = {
        "d908aab5-7a90-4cbe-8641-86a553db-X",
        std::string(GalaxyBudsHelper::GALAXYBUDSNEW),
    };
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds2 &&
           pairModelColor.second == "";
}

bool TestsSgb::TestFindByName1()
{
    std::vector<std::string> guids = {};
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "Samsung Galaxy Buds+ (user)");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBudsPlus &&
           pairModelColor.second == "";
}

bool TestsSgb::TestFindByName2()
{
    std::vector<std::string> guids = {};
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "Galaxy Buds2 (user)");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds2 &&
           pairModelColor.second == "";
}

bool TestsSgb::TestFindByName3()
{
    std::vector<std::string> guids = {};
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "Samsung Galaxy Buds2 Pro (user))");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds2Pro &&
           pairModelColor.second == "";
}

bool TestsSgb::TestFindByName4()
{
    std::vector<std::string> guids = {};
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "Galaxy Buds3 (user)");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds3 &&
           pairModelColor.second == "";
}

bool TestsSgb::TestFindByName5()
{
    std::vector<std::string> guids = {};
    std::pair<GalaxyBudsModelIds, std::string> pairModelColor = GalaxyBudsHelper::SearchModelColor(guids, "Samsung Galaxy Buds3 Pro (user)");

    return pairModelColor.first == GalaxyBudsModelIds::GalaxyBuds3Pro &&
           pairModelColor.second == "";
}

bool TestsSgb::TestExtract1()
{
    std::vector<unsigned char> raw = {253, 61, 0, 97, 2, 8, 100, 100, 1, 1, 17, 0, 0, 0, 255, 34, 0, 0, 84, 1, 84, 1, 7, 0, 4, 221, 0, 4, 4, 16, 0, 1, 0, 0, 17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 255, 1, 1, 0, 15, 84, 221};
    std::vector<unsigned char> expectedPayload = {2, 8, 100, 100, 1, 1, 17, 0, 0, 0, 255, 34, 0, 0, 84, 1, 84, 1, 7, 0, 4, 221, 0, 4, 4, 16, 0, 1, 0, 0, 17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 255, 1, 1, 0};

    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (optdata.has_value())
    {
        GalaxyBudsResponseData data = optdata.value();
        if (data.Id == GalaxyBudsMsgIds::EXTENDED_STATUS_UPDATED &&
            data.Type == GalaxyBudsMsgTypes::Response &&
            data.Payload == expectedPayload)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool TestsSgb::TestEncode1()
{
    std::vector<unsigned char> payload = {0};
    std::vector<unsigned char> expectedData = {0, 4, 0, 120, 0, 240, 129, 0};

    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::vector<unsigned char> data = packet.Encode(GalaxyBudsMsgIds::NOISE_CONTROLS, payload);

    if (data == expectedData)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool TestsSgb::TestAnc1()
{
    std::vector<unsigned char> raw = {253, 61, 192, 97, 2, 8, 100, 100, 1, 1, 33, 0, 0, 0, 255, 34, 2, 0, 84, 1, 84, 1, 7, 0, 4, 221, 0, 4, 4, 16, 0, 1, 0, 0, 17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 255, 1, 1, 0, 212, 72, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsAncWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<GalaxyBudsAnc> ancPromise;
    std::future<GalaxyBudsAnc> ancFuture = ancPromise.get_future();
    size_t listenerId = watcher.GetAncChangedEvent().Subscribe([&ancPromise](size_t, const GalaxyBudsAnc &ancData)
                                                                   { ancPromise.set_value(ancData); });

    watcher.ProcessResponse(data);
    GalaxyBudsAnc ancData = ancFuture.get();

    watcher.GetAncChangedEvent().Unsubscribe(listenerId);

    return ancData == GalaxyBudsAnc::AmbientSound;
}

bool TestsSgb::TestAnc2()
{
    std::vector<unsigned char> raw = {253, 61, 128, 97, 2, 8, 100, 100, 1, 1, 17, 0, 0, 0, 255, 34, 0, 0, 84, 1, 84, 1, 7, 0, 4, 221, 0, 4, 4, 16, 0, 1, 0, 0, 17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 255, 1, 1, 0, 15, 84, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsAncWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<GalaxyBudsAnc> ancPromise;
    std::future<GalaxyBudsAnc> ancFuture = ancPromise.get_future();
    size_t listenerId = watcher.GetAncChangedEvent().Subscribe([&ancPromise](size_t, const GalaxyBudsAnc &ancData)
                                                                   { ancPromise.set_value(ancData); });

    watcher.ProcessResponse(data);
    GalaxyBudsAnc ancData = ancFuture.get();

    watcher.GetAncChangedEvent().Unsubscribe(listenerId);

    return ancData == GalaxyBudsAnc::Off;
}

bool TestsSgb::TestAnc3()
{
    std::vector<unsigned char> raw = {253, 10, 0, 119, 0, 17, 1, 0, 13, 13, 1, 77, 166, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsAncWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<GalaxyBudsAnc> ancPromise;
    std::future<GalaxyBudsAnc> ancFuture = ancPromise.get_future();
    size_t listenerId = watcher.GetAncChangedEvent().Subscribe([&ancPromise](size_t, const GalaxyBudsAnc &ancData)
                                                                   { ancPromise.set_value(ancData); });

    watcher.ProcessResponse(data);
    GalaxyBudsAnc ancData = ancFuture.get();

    watcher.GetAncChangedEvent().Unsubscribe(listenerId);

    return ancData == GalaxyBudsAnc::Off;
}

bool TestsSgb::TestAnc4()
{
    std::vector<unsigned char> raw = {253, 10, 64, 119, 1, 17, 1, 0, 13, 13, 1, 44, 30, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsAncWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<GalaxyBudsAnc> ancPromise;
    std::future<GalaxyBudsAnc> ancFuture = ancPromise.get_future();
    size_t listenerId = watcher.GetAncChangedEvent().Subscribe([&ancPromise](size_t, const GalaxyBudsAnc &ancData)
                                                                   { ancPromise.set_value(ancData); });

    watcher.ProcessResponse(data);
    GalaxyBudsAnc ancData = ancFuture.get();

    watcher.GetAncChangedEvent().Unsubscribe(listenerId);

    return ancData == GalaxyBudsAnc::NoiseReduction;
}

bool TestsSgb::TestEarDetection1()
{
    std::vector<unsigned char> raw = {253, 61, 0, 97, 2, 8, 100, 100, 1, 1, 17, 0, 0, 0, 255, 34, 0, 0, 84, 1, 84, 1, 7, 0, 4, 221, 0, 4, 4, 16, 0, 1, 0, 0, 17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 255, 1, 1, 0, 15, 84, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsEarDetectionWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<GalaxyBudsEarDetectionStateArgs> promise;
    std::future<GalaxyBudsEarDetectionStateArgs> future = promise.get_future();
    size_t listenerId = watcher.GetEarDetectionStateChangedEvent().Subscribe([&promise](size_t, const GalaxyBudsEarDetectionStateArgs &state)
                                                                             { promise.set_value(state); });

    watcher.ProcessResponse(data);
    GalaxyBudsEarDetectionStateArgs state = future.get();

    watcher.GetEarDetectionStateChangedEvent().Unsubscribe(listenerId);

    return state.Left == GalaxyBudsEarDetectionState::Wearing && state.Right == GalaxyBudsEarDetectionState::Wearing;
}

bool TestsSgb::TestEarDetection2()
{
    std::vector<unsigned char> raw = {253, 11, 192, 96, 1, 100, 100, 1, 1, 17, 30, 0, 89, 28, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsEarDetectionWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<GalaxyBudsEarDetectionStateArgs> promise;
    std::future<GalaxyBudsEarDetectionStateArgs> future = promise.get_future();
    size_t listenerId = watcher.GetEarDetectionStateChangedEvent().Subscribe([&promise](size_t, const GalaxyBudsEarDetectionStateArgs &state)
                                                                             { promise.set_value(state); });

    watcher.ProcessResponse(data);
    GalaxyBudsEarDetectionStateArgs state = future.get();

    watcher.GetEarDetectionStateChangedEvent().Unsubscribe(listenerId);

    return state.Left == GalaxyBudsEarDetectionState::Wearing && state.Right == GalaxyBudsEarDetectionState::Wearing;
}

bool TestsSgb::TestEarDetection3()
{
    std::vector<unsigned char> raw = {253, 11, 8, 96, 1, 100, 99, 1, 1, 51, 87, 4, 110, 203, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsEarDetectionWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<GalaxyBudsEarDetectionStateArgs> promise;
    std::future<GalaxyBudsEarDetectionStateArgs> future = promise.get_future();
    size_t listenerId = watcher.GetEarDetectionStateChangedEvent().Subscribe([&promise](size_t, const GalaxyBudsEarDetectionStateArgs &state)
                                                                             { promise.set_value(state); });

    watcher.ProcessResponse(data);
    GalaxyBudsEarDetectionStateArgs state = future.get();

    watcher.GetEarDetectionStateChangedEvent().Unsubscribe(listenerId);

    return state.Left == GalaxyBudsEarDetectionState::Case && state.Right == GalaxyBudsEarDetectionState::Case;
}

bool TestsSgb::TestSetAnc1()
{
    std::vector<unsigned char> raw = {253, 10, 64, 119, 1, 17, 1, 0, 13, 13, 1, 44, 30, 221};
    std::vector<unsigned char> expected = {253, 4, 0, 120, 2, 178, 161, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsSetAnc stage = GalaxyBudsSetAnc(GalaxyBudsAnc::AmbientSound);
    return packet.Encode(stage.Id, stage.Payload) == expected;
}

bool TestsSgb::TestSetAnc2()
{
    std::vector<unsigned char> raw = {253, 10, 64, 119, 1, 17, 1, 0, 13, 13, 1, 44, 30, 221};
    std::vector<unsigned char> expected = {253, 4, 0, 120, 3, 147, 177, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsSetAnc stage = GalaxyBudsSetAnc(GalaxyBudsAnc::Adaptive);
    return packet.Encode(stage.Id, stage.Payload) == expected;
}

bool TestsSgb::TestBattery1()
{
    std::vector<unsigned char> raw = {253, 61, 0, 97, 2, 8, 100, 100, 1, 1, 17, 0, 0, 0, 255, 34, 0, 0, 84, 1, 84, 1, 7, 0, 4, 221, 0, 4, 4, 16, 0, 1, 0, 0, 17, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 255, 1, 1, 0, 15, 84, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsBatteryWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<std::vector<DeviceBatteryData>> promise;
    std::future<std::vector<DeviceBatteryData>> future = promise.get_future();
    size_t listenerId = watcher.GetBatteryChangedEvent().Subscribe([&promise](size_t, const std::vector<DeviceBatteryData> &state)
                                                                   { promise.set_value(state); });

    watcher.ProcessResponse(data);
    std::vector<DeviceBatteryData> state = future.get();

    watcher.GetBatteryChangedEvent().Unsubscribe(listenerId);

    bool isLeftPass, isRightPass, isCasePass = false;
    for (auto battery : state)
    {
        if (battery.Type == DeviceBatteryType::Left)
        {
            if (battery.Battery == 100 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isLeftPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Right)
        {
            if (battery.Battery == 100 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isRightPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Case)
        {
            if (battery.Battery == 0 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Disconnected)
                isCasePass = true;
        }
    }

    return isLeftPass && isRightPass && isCasePass;
}

bool TestsSgb::TestBattery2()
{
    std::vector<unsigned char> raw = {253, 11, 64, 96, 1, 99, 99, 1, 0, 17, 33, 0, 67, 174, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBuds3Pro);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsBatteryWatcher watcher(GalaxyBudsModelIds::GalaxyBuds3Pro);

    std::promise<std::vector<DeviceBatteryData>> promise;
    std::future<std::vector<DeviceBatteryData>> future = promise.get_future();
    size_t listenerId = watcher.GetBatteryChangedEvent().Subscribe([&promise](size_t, const std::vector<DeviceBatteryData> &state)
                                                                   { promise.set_value(state); });

    watcher.ProcessResponse(data);
    std::vector<DeviceBatteryData> state = future.get();

    watcher.GetBatteryChangedEvent().Unsubscribe(listenerId);

    bool isLeftPass, isRightPass, isCasePass = false;
    for (auto battery : state)
    {
        if (battery.Type == DeviceBatteryType::Left)
        {
            if (battery.Battery == 99 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isLeftPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Right)
        {
            if (battery.Battery == 99 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isRightPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Case)
        {
            if (battery.Battery == 33 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isCasePass = true;
        }
    }

    return isLeftPass && isRightPass && isCasePass;
}

bool TestsSgb::TestBattery3()
{
    std::vector<unsigned char> raw = {253, 11, 8, 96, 1, 100, 99, 1, 1, 51, 87, 4, 110, 203, 221};
    GalaxyBudsPacket packet(GalaxyBudsModelIds::GalaxyBudsFe);
    std::optional<GalaxyBudsResponseData> optdata = packet.Extract(raw);

    if (!optdata.has_value())
        return false;

    GalaxyBudsResponseData data = optdata.value();
    GalaxyBudsBatteryWatcher watcher(GalaxyBudsModelIds::GalaxyBudsFe);

    std::promise<std::vector<DeviceBatteryData>> promise;
    std::future<std::vector<DeviceBatteryData>> future = promise.get_future();
    size_t listenerId = watcher.GetBatteryChangedEvent().Subscribe([&promise](size_t, const std::vector<DeviceBatteryData> &state)
                                                                   { promise.set_value(state); });

    watcher.ProcessResponse(data);
    std::vector<DeviceBatteryData> state = future.get();

    watcher.GetBatteryChangedEvent().Unsubscribe(listenerId);

    bool isLeftPass, isRightPass, isCasePass = false;
    for (auto battery : state)
    {
        if (battery.Type == DeviceBatteryType::Left)
        {
            if (battery.Battery == 100 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isLeftPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Right)
        {
            if (battery.Battery == 99 && battery.IsCharging == true && battery.Status == DeviceBatteryStatus::Connected)
                isRightPass = true;
        }
        else if (battery.Type == DeviceBatteryType::Case)
        {
            if (battery.Battery == 87 && battery.IsCharging == false && battery.Status == DeviceBatteryStatus::Connected)
                isCasePass = true;
        }
    }

    return isLeftPass && isRightPass && isCasePass;
}

void TestsSgb::Test(const char *name, bool b)
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

MagicPodsCore::TestsSgb::TestsSgb()
{
    Test("CRC16.Checksum1", TestChecksum1());
    Test("CRC16.Checksum2", TestChecksum2());
    Test("GalaxyBudsHelper.FindByGuid1", TestFindByGuid1());
    Test("GalaxyBudsHelper.FindByGuid2", TestFindByGuid2());
    Test("GalaxyBudsHelper.FindByGuid3", TestFindByGuid3());
    Test("GalaxyBudsHelper.FindByGuid4", TestFindByGuid4());
    Test("GalaxyBudsHelper.FindByName1", TestFindByName1());
    Test("GalaxyBudsHelper.FindByName2", TestFindByName2());
    Test("GalaxyBudsHelper.FindByName3", TestFindByName3());
    Test("GalaxyBudsHelper.FindByName4", TestFindByName4());
    Test("GalaxyBudsHelper.FindByName5", TestFindByName5());
    Test("GalaxyBudsPacket.TestExtract1", TestExtract1());
    Test("GalaxyBudsPacket.TestEncode1", TestEncode1());
    Test("GalaxyBudsAncWatcher.TestAnc1", TestAnc1());
    Test("GalaxyBudsAncWatcher.TestAnc2", TestAnc2());
    Test("GalaxyBudsAncWatcher.TestAnc3", TestAnc3());
    Test("GalaxyBudsAncWatcher.TestAnc4", TestAnc4());
    Test("GalaxyBudsEarDetectionWatcher.TestEarDetection1", TestEarDetection1());
    Test("GalaxyBudsEarDetectionWatcher.TestEarDetection3", TestEarDetection2());
    Test("GalaxyBudsEarDetectionWatcher.TestEarDetection2", TestEarDetection3());
    Test("GalaxyBudsSetAnc.TestSetAnc1", TestSetAnc1());
    Test("GalaxyBudsSetAnc.TestSetAnc2", TestSetAnc2());
    Test("GalaxyBudsBatteryWatcher.TestBattery1", TestBattery1());
    Test("GalaxyBudsBatteryWatcher.TestBattery2", TestBattery2());
    Test("GalaxyBudsBatteryWatcher.TestBattery3", TestBattery3());
    //std::string modalias = "bluetooth:v004Cp200EdB087";
    //size_t vi = modalias.find("v");
    //size_t pi = modalias.find("p");
    //size_t di = modalias.find("d");
}