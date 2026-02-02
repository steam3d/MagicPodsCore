// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "TestsAapBle.h"
#include "device/capabilities/aap/AppAnimationCapability.h"
#include "StringUtils.h"
#include "Logger.h"
#include <cstring>
#include <optional>
#include <string>

using namespace MagicPodsCore;

TestsAapBle::TestsAapBle()
{
    std::string enc = "";

    Test("TestAirPodsMaxInEar_utp_22", TestAirPodsMaxInEar_utp_22());
    Test("TestAirPodsMaxPopupAnimation", TestAirPodsMaxPopupAnimation());
    Test("TestAirPods2_utp_53", TestAirPods2_utp_53());
    Test("TestAirPods2_utp_33", TestAirPods2_utp_33());
    Test("TestBeatsSolo4", TestBeatsSolo4());
    Test("TestBeatsSoloBuds", TestBeatsSoloBuds());
    Test("TestPowerBeatsPro2", TestPowerBeatsPro2());
    Test("TestBeatsStudioBudsPlus", TestBeatsStudioBudsPlus());
    Test("TestBeatsStudioPro", TestBeatsStudioPro());
    Test("TestPowerBeatsPro", TestPowerBeatsPro());
    Test("TestPowerBeats4", TestPowerBeats4());
    Test("TestPrivateAirPods2_1", TestPrivateAirPods2_1(enc));
    Test("TestPrivateAirPods2_2", TestPrivateAirPods2_2(enc));
    Test("TestPrivateAirPods2_3", TestPrivateAirPods2_3(enc));
    Test("TestPrivateAirPods2_4", TestPrivateAirPods2_4(enc));
    Test("TestPrivateAirPods2_5", TestPrivateAirPods2_5(enc));
    Test("TestPrivateAirPods2_6", TestPrivateAirPods2_6(enc));
    Test("TestPrivateAirPods2_7", TestPrivateAirPods2_7(enc));
    Test("TestPrivateAirPods2_8", TestPrivateAirPods2_8(enc));
}

bool TestsAapBle::TestAirPodsMaxInEar_utp_22()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x0f;
    expected.model = AapModelIds::airpodsmax;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Single,
        DeviceBatteryStatus::Connected,
        50,
        false);

    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010a20220580000f45e80d9bb8e51897326a99455bb1cff367"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestAirPodsMaxPopupAnimation()
{
    bleData expected;
    expected.animation = true;
    expected.color = 0x0f;
    expected.model = AapModelIds::airpodsmax;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Single,
        DeviceBatteryStatus::Connected,
        40,
        false);

    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010a20020480820f400185c65a0aff9097826bd7c542e1cc55"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestAirPods2_utp_53()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        90,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        60,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f2053a9960200050185c65a0aff9097826bd7c542e1cc55"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestAirPods2_utp_33()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        90,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        60,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f20339aa60100050185c65a0aff9097826bd7c542e1cc55"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestBeatsSolo4()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::beatssolo4;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Single,
        DeviceBatteryStatus::Connected,
        90,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("07190125200009800400041ed3edebd0bc052b11618fc29f861d8a"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestBeatsSoloBuds()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x03;
    expected.model = AapModelIds::beatssolobuds;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        90,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        80,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("071901262033898001030496bfb2c5f7b2365d80676c748ebcda28"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPowerBeatsPro2()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x01;
    expected.model = AapModelIds::powerbeatspro2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        90,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        100,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Disconnected,
        0,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719011d20019a8f02010489f29d3a59d0fb608af4215871356c34"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestBeatsStudioBudsPlus()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::beatsstudiobudsplus;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Disconnected,
        0,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        10,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("07190116203cfa01060000db15f83d01f00da7fa30b667a1313ecd"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestBeatsStudioPro()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x01;
    expected.model = AapModelIds::beatsstudiopro;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Single,
        DeviceBatteryStatus::Connected,
        60,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719011720000600010100eb63a515bf0f416a06574cf37bc55a22"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPowerBeatsPro()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x1f;
    expected.model = AapModelIds::powerbeatspro;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        100,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        50,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010b2013aaa5011f255b2524f9b6d960d3b047ff447133b345"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPowerBeats4()
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x01;
    expected.model = AapModelIds::powerbeats4;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Single,
        DeviceBatteryStatus::Connected,
        70,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010d20000780050105d8c07399be0547113083d7b84bde576e"), static_cast<unsigned short>(expected.model));
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_1(const std::string &enc)
{
    // extract private failed
    bleData expected;
    expected.animation = true;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        100,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        70,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f2075aab7010000fc5e182d7502ec13733292d933061b3c"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_2(const std::string &enc)
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        100,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        72,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f2071aa9709000439e77f8663dcc46c531ef4f2869e1473"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_3(const std::string &enc)
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        99,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        72,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f20719a9709000418b9477e7df5caab57f7bdfc05bad3c4"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_4(const std::string &enc)
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Disconnected,
        0,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        99,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Disconnected,
        0,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f2000f98f010004a8d828263f10af19a01375e3cac56acc"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_5(const std::string &enc)
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        99,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        99,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        72,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f205199970a00044ab2fd1614bc36a17df1b5a22a5556e6"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_6(const std::string &enc)
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        99,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Disconnected,
        0,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Disconnected,
        0,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f2021f98f0100042ac1d294f37c7b5f596ebd6876a5c2c6"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_7(const std::string &enc)
{
    bleData expected;
    expected.animation = false;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        99,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        99,
        false);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Disconnected,
        0,
        false);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f2021998f0100044f8e71ff6d69604976eb7eb8cc959e59"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

bool TestsAapBle::TestPrivateAirPods2_8(const std::string &enc)
{
    bleData expected;
    expected.animation = true;
    expected.color = 0x00;
    expected.model = AapModelIds::airpods2;
    expected.batteryData.emplace_back(
        DeviceBatteryType::Left,
        DeviceBatteryStatus::Connected,
        100,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Right,
        DeviceBatteryStatus::Connected,
        100,
        true);
    expected.batteryData.emplace_back(
        DeviceBatteryType::Case,
        DeviceBatteryStatus::Connected,
        71,
        true);
    std::optional<bleData> actual = AppAnimationCapability::ParseBle(StringUtils::HexStringToBytes("0719010f2055aaf701000474a70931b203722fe2d40dda81d0e64c"), static_cast<unsigned short>(expected.model), enc);
    return (actual.has_value() && actual.value() == expected);
}

void TestsAapBle::Test(const char *name, bool b)
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
