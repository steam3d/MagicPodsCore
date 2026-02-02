// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>

namespace MagicPodsCore
{
    class TestsAapBle
    {
    private:
        bool TestAirPodsMaxInEar_utp_22();
        bool TestAirPodsMaxPopupAnimation();
        bool TestAirPods2_utp_53();
        bool TestAirPods2_utp_33();
        bool TestBeatsSolo4();
        bool TestBeatsSoloBuds();
        bool TestPowerBeatsPro2();
        bool TestBeatsStudioBudsPlus();
        bool TestBeatsStudioPro();
        bool TestPowerBeatsPro();
        bool TestPowerBeats4();
        bool TestPrivateAirPods2_1(const std::string &enc);
        bool TestPrivateAirPods2_2(const std::string &enc);
        bool TestPrivateAirPods2_3(const std::string &enc);
        bool TestPrivateAirPods2_4(const std::string &enc);
        bool TestPrivateAirPods2_5(const std::string &enc);
        bool TestPrivateAirPods2_6(const std::string &enc);
        bool TestPrivateAirPods2_7(const std::string &enc);
        bool TestPrivateAirPods2_8(const std::string &enc);
        void Test(const char *name, bool b);

    public:
        TestsAapBle();
    };
}
