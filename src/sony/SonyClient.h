#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "SonyBaseCmd.h"

namespace MagicPodsCore
{
    //CLASS NOT TESTED
    class SonyClient
    {
    protected:
        std::string _tag{};

    public:
        unsigned char Prefix;
        explicit SonyClient(std::string tag);
        void WaitResult(SonyBaseCmd &cmd, int timeOutSeconds = 2);
    private:
        void ProcessResponse(const std::vector<unsigned char> &bytes);
    };
}