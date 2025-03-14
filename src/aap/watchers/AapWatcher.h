#pragma once

#include "../../Logger.h"
#include <string>
#include <vector>

namespace MagicPodsCore
{
    struct WatcherData {
        std::string Tag{};
    };
    
    // Base class to process incoming data from headphones
    class AapWatcher
    {
    protected:
        std::string _tag{};

    public:
        AapWatcher(std::string tag);
        // Subscribe each class to process incoming data from headphones
        virtual void ProcessBytes(const std::vector<unsigned char> &bytes) = 0;
    };

}