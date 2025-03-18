#pragma once

#include "../../Logger.h"
#include <string>
#include <vector>

namespace MagicPodsCore
{    
    // Base class to process incoming data from headphones
    class AapWatcher
    {
    protected:
        std::string _tag{};

    public:
        AapWatcher(std::string tag);
        // Subscribe each class to process incoming data from headphones
        virtual void ProcessResponse(const std::vector<unsigned char> &data) = 0;
    };

}