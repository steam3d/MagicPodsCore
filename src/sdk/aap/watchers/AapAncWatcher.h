#pragma once

#include "Event.h"
#include "sdk/aap/enums/AapAncMode.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    class AapAncWatcher : public AapWatcher
    {
    private:
        Event<AapAncMode> _event{};

    public:
        AapAncWatcher();
        void ProcessResponse(const std::vector<unsigned char> &data) override;

        Event<AapAncMode> &GetEvent()
        {
            return _event;
        }
    };

}