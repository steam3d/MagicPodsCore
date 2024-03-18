#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "SonyBaseWatcher.h"
#include "SonyGetAnc.h"
#include "../DeviceBattery.h"

namespace MagicPodsCore
{
    class SonyAncWatcher : public SonyBaseWatcher
    {
    public:
        SonyAncState State{};
        SonyAncWatcher();
        void ProcessResponse(const std::vector<unsigned char> &bytes) override;
        Event<SonyAncState> &GetEvent()
        {
            return _event;
        }

    private:
        Event<SonyAncState> _event{};
    };
}
