#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "SonyGetAnc.h"

namespace MagicPodsCore
{
    class SonyAncHelper
    {
    public:
        static std::string DummyConvertSonyAncSwitch(SonyAncSwitch status);
        static std::string DummyConvertSonyAncFilter(SonyAncFilter status);
        static std::string DummyConvertSonyAncFilterAmbientVoice(SonyAncFilterAmbientVoice status);
    };
}