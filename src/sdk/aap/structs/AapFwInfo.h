// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <sstream>
#include <string>

namespace MagicPodsCore
{
    struct AapFwInfo
    {
        std::string Name{};
        std::string Model{};
        std::string Manufacturer{};
        std::string Serial{}; // case and airpods max?
        std::string Ver1{};
        std::string Ver2{};
        std::string HwRev{};
        std::string UpdaterId{};
        std::string LeftSerial{};
        std::string RightSerial{};
        std::string Ver3{};

        std::string ToString() const
        {
            std::ostringstream os;
            os << "Name: " << Name
               << " Model number: " << Model
               << " Manufacturer: " << Manufacturer
               << " Serial number: " << Serial
               << " Version 1: " << Ver1
               << " Version 2: " << Ver2
               << " Hardware revision: " << HwRev
               << " Updater identifier: " << UpdaterId
               << " Left serial number: " << LeftSerial
               << " Right serial number: " << RightSerial
               << " Version 3: " << Ver3;

            return os.str();
        }
    };
}
