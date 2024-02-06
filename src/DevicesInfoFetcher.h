#pragma once

#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <set>

#include <sdbus-c++/sdbus-c++.h>
#include <json.hpp>

#include "Device.h"

namespace MagicPodsCore {

    class DeviceComparator {
    public:
        bool operator()(const std::shared_ptr<Device>& device1, const std::shared_ptr<Device>& device2) const {
            return device1->GetAddress() < device2->GetAddress();
        }
    };

    class DevicesInfoFetcher {
    private:
        std::unique_ptr<sdbus::IProxy> _rootProxy{};
        std::set<std::shared_ptr<Device>, DeviceComparator> _devices{};

    public:
        DevicesInfoFetcher();
        // TODO: запретить копирование и перенос

        std::set<std::shared_ptr<Device>> GetAirpodsInfos();

        void Connect(const std::string& deviceAddress);

        void Disconnect(const std::string& deviceAddress);

        std::string AsJson();

    private:
        void UpdateInfos();
        std::set<std::shared_ptr<Device>, DeviceComparator> LoadActualDevices();

        void OnDevicesAdd(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices);
        void OnDevicesRemove(const std::set<std::shared_ptr<Device>, DeviceComparator>& devices);
    };

}
