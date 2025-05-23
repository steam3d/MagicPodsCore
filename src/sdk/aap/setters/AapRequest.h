// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Logger.h"
#include <string>
#include <vector>

namespace MagicPodsCore
{

    // Base class to get request that will be send to headphones
    class AapRequest
    {
    protected:
        std::string _tag{};

    public:
        explicit AapRequest(std::string tag);
        virtual std::vector<unsigned char> Request() const = 0;
        void PrintAsHex();
    };
}