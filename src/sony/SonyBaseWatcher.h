#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

namespace MagicPodsCore
{
    class SonyBaseWatcher
    {
    protected:
        std::string _tag{};

    public:
        explicit SonyBaseWatcher(std::string tag);                
        virtual void ProcessResponse(const std::vector<unsigned char> &bytes) = 0;
    };
}