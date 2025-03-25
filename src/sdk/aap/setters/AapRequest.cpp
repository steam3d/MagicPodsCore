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
        LOG_DEBUG("%s: %s", _tag.c_str(), StringUtils::BytesToHexString(bytes).c_str());
    }
}