#include "SonyBaseWatcher.h"
#include "Logger.h"

namespace MagicPodsCore
{
    SonyBaseWatcher::SonyBaseWatcher(std::string tag) : _tag{tag}
    {
    }

    
    void SonyBaseWatcher::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        LOG_DEBUG("Watcher %s processed", _tag.c_str());
    }
}