// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsSetAnc.h"
#include "sdk/pbp/PixelBudsHelper.h"
#include "sdk/pbp/ProtoWire.h"

namespace MagicPodsCore
{
    namespace
    {
        std::vector<unsigned char> EncodeWriteSettingMsg(PixelBudsAncState state)
        {
            std::vector<unsigned char> settingValue;
            ProtoWire::WriteVarintField(settingValue, PixelBudsHelper::CurrentAncrStateValueField, static_cast<uint64_t>(state));

            std::vector<unsigned char> writeSettingMsg;
            ProtoWire::WriteBytesField(writeSettingMsg, PixelBudsHelper::WriteSettingValueField, settingValue);

            return writeSettingMsg;
        }
    }

    PixelBudsSetAnc::PixelBudsSetAnc(PixelBudsAncState state)
        : MethodId(PixelBudsHelper::WriteSettingMethodId),
          Payload(EncodeWriteSettingMsg(state)) {}
}
