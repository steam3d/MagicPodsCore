#pragma once
#include <string>
#include "SonyEnums.h"

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