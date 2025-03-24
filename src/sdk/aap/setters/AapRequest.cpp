#include "AapRequest.h"

namespace MagicPodsCore
{
    AapRequest::AapRequest(std::string tag) : _tag{tag}
    {
    }

    void AapRequest::PrintAsHex()
    {
        LOG_DEBUG("%s ", _tag.c_str());
        std::vector<unsigned char> bytes = Request();
        for (int i = 0; i < bytes.size(); i++)
            LOG_DEBUG("%02x", bytes[i]);
        LOG_DEBUG("");
        LOG_DEBUG("");
    }
}