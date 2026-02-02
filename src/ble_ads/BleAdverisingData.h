// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>

namespace MagicPodsCore
{

    struct BleAdertisingData
    {
    private:
        std::string _address;
        int8_t _rssi;
        std::map<uint16_t, std::vector<uint8_t>> _manufacturerData;

    public:
        BleAdertisingData(std::string &address, int8_t rssi, std::map<uint16_t, std::vector<uint8_t>> manufacturerData) : _address(std::move(address)), _rssi(rssi), _manufacturerData(std::move(manufacturerData))
        {
        }

        const std::string& GetAddress() const {
            return _address;
        }

        int8_t GetRssi() const {
            return _rssi;
        }

        const std::map<uint16_t, std::vector<uint8_t>>& GetManufacturerData() const {
            return _manufacturerData;
        }
    };

}