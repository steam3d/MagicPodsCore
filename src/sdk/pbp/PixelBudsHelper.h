// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

// The Maestro protocol of the Google Pixel Buds is not documented by the vendor. The framing, the
// address and channel tables, the Pigweed RPC id hash and the message layout implemented in
// src/sdk/pbp are a reimplementation of the reverse engineering work done by Maximilian Luz in
// pbpctrl (https://github.com/qzed/pbpctrl, Apache-2.0 / MIT).

#pragma once

#include "StringUtils.h"
#include "Hash65599.h"
#include "sdk/pbp/enums/PixelBudsModelIds.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace MagicPodsCore
{
    class PixelBudsHelper
    {
    public:
        static bool IsPixelBudsDevice(const std::vector<std::string> &guids);
        static PixelBudsModelIds SearchModel(const std::vector<std::string> &guids, unsigned short vendorId, unsigned short productId);
        static std::string GetServiceGuid(PixelBudsModelIds model);

        inline static const std::string MAESTRO = "25e97ff7-24ce-4c4c-8951-f764a708f7b5";
        // GFPS Message Stream is used by Pixel Buds A-Series instead of Maestro.
        // inline static const std::string GFPS = "df21fe2c-2515-4fdb-8886-f12c4d67927c";

        // Pigweed RPC ids of the maestro service and of the methods we use
        inline static constexpr uint32_t MaestroServiceId = Hash65599::Hash("maestro_pw.Maestro");
        inline static constexpr uint32_t GetSoftwareInfoMethodId = Hash65599::Hash("GetSoftwareInfo");
        inline static constexpr uint32_t SubscribeRuntimeInfoMethodId = Hash65599::Hash("SubscribeRuntimeInfo");
        inline static constexpr uint32_t ReadSettingMethodId = Hash65599::Hash("ReadSetting");
        inline static constexpr uint32_t WriteSettingMethodId = Hash65599::Hash("WriteSetting");
        inline static constexpr uint32_t SubscribeToSettingsChangesMethodId = Hash65599::Hash("SubscribeToSettingsChanges");

        // Call id used while the channel of the buds is unknown
        inline static constexpr uint32_t ChannelProbeCallId = 0xFFFFFFFF;

        // Field numbers of the settings messages
        inline static constexpr unsigned int ReadSettingIdField = 4;         // ReadSettingMsg.settings_id
        inline static constexpr unsigned int WriteSettingValueField = 4;     // WriteSettingMsg.setting
        inline static constexpr unsigned int SettingsRspValueField = 4;      // SettingsRsp.value
        inline static constexpr unsigned int CurrentAncrStateValueField = 13; // SettingValue.current_ancr_state

        // maestro_pw.AllegroSettingType
        inline static constexpr uint64_t CurrentAncrStateSettingId = 13; // ALLEGRO_CURRENT_ANCR_STATE

    private:
        static PixelBudsModelIds GetModelFromVidPid(unsigned short vendorId, unsigned short productId);
    };
}
