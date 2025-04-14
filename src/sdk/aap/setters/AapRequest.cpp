// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapRequest.h"
#include "StringUtils.h"

namespace MagicPodsCore
{
    AapRequest::AapRequest(std::string tag) : _tag{std::move(tag)}
    {
    }

    void AapRequest::PrintAsHex()
    {
        std::vector<unsigned char> bytes = Request();
        Logger::Debug("%s: %s", _tag.c_str(), StringUtils::BytesToHexString(bytes).c_str());
    }
}