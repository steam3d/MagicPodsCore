// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once
#include <string>
#include <vector>

namespace MagicPodsCore
{
    class Aes
    {
    public:
        static bool VerifyRPA(const std::string &address, const std::string &irk);
        static std::vector<unsigned char> DecryptPayload(const std::string &encPayload, const std::string &enc);
        static std::vector<unsigned char> DecryptPayload(const std::vector<unsigned char> &encPayload, const std::vector<unsigned char> &enc);
    };
}