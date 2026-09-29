// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore{
    class TestsPbp
    {
    private:
        bool TestChecksum1();
        bool TestChecksum2();
        bool TestVarint1();
        bool TestVarint2();
        bool TestVarint3();
        bool TestVarint4();
        bool TestVarint5();
        bool TestHash1();
        bool TestHash2();
        bool TestAddress1();
        bool TestAddress2();
        bool TestEncode1();
        bool TestExtract1();
        bool TestExtractChunks1();
        bool TestExtractChecksum1();
        bool TestRpcPacket1();
        bool TestRpcPacket2();
        bool TestChannelProbe1();
        bool TestBattery1();
        bool TestBattery2();
        bool TestBattery3();
        bool TestBattery4();
        bool TestAnc1();
        bool TestAnc2();
        bool TestAnc3();
        bool TestSetAnc1();
        bool TestFindByClass1();
        bool TestFindByClass2();
        bool TestFindByName1();
        bool TestFindByName2();
        bool TestFindByName3();
        void Test(const char *name, bool b);
    public:
        TestsPbp();
    };
}
