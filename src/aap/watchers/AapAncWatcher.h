#pragma once

#include "../../Event.h"
#include "../enums/AapAncMode.h"
#include "../enums/AapCmd.h"
#include "../enums/AapCmdSettings.h"
#include "AapWatcher.h"

namespace MagicPodsCore
{
    struct AncWatcherData : public WatcherData
    {
        AapAncMode Mode{};
    };

    class AapAncWatcher : public AapWatcher
    {
    private:
        Event<AncWatcherData> _event{};

    public:
        AapAncWatcher();
        void ProcessBytes(const std::vector<unsigned char> &bytes) override;

        Event<AncWatcherData> &GetEvent()
        {
            return _event;
        }

    private:
        std::string DummyConvertAncMode(AapAncMode mode);
    };

}