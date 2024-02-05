#pragma once

#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <map>

#include <sdbus-c++/sdbus-c++.h>
#include <json.hpp>

#include "Device.h"

namespace MagicPodsCore {

    class DevicesInfoFetcher {
    private:
        std::unique_ptr<sdbus::IProxy> _rootProxy{};
        std::vector<std::shared_ptr<Device>> _devices{};

    public:
        DevicesInfoFetcher();
        // TODO: запретить копирование и перенос

        std::vector<std::shared_ptr<Device>> GetAirpodsInfos();

        void Connect(const std::string& deviceAddress);

        void Disconnect(const std::string& deviceAddress);

        std::string AsJson();

    private:
        void UpdateInfos();
    };

}
