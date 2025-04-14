// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

namespace MagicPodsCore{
    class TestsSgb
    {
    private:
        bool TestChecksum1();
        bool TestChecksum2();
        bool TestFindByGuid1();
        bool TestFindByGuid2();
        bool TestFindByGuid3();
        bool TestFindByGuid4();
        bool TestFindByName1();
        bool TestFindByName2();
        bool TestFindByName3();
        bool TestFindByName4();
        bool TestFindByName5();
        bool TestExtract1();
        bool TestEncode1();
        bool TestAnc1();
        bool TestAnc2();
        bool TestAnc3();
        bool TestAnc4();
        bool TestEarDetection1();
        bool TestEarDetection2();
        bool TestEarDetection3();
        bool TestSetAnc1();
        bool TestSetAnc2();
        bool TestBattery1();
        bool TestBattery2();
        bool TestBattery3();
        void Test(const char *name, bool b);
    public:
        TestsSgb();        
    };    
}
