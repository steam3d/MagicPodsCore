#pragma once

#include "../../Event.h"
#include "../enums/AapAncMode.h"
#include "../enums/AapCmd.h"
#include "../enums/AapCmdSettings.h"
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

    private:
        std::string DummyConvertAncMode(AapAncMode mode);
    };

}