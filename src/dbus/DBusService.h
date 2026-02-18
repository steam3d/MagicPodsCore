// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "DBusDeviceInfo.h"
#include "ObservableVariable.h"

#include <memory>
#include <map>
#include <set>
#include <vector>
#include <regex>
#include <sdbus-c++/sdbus-c++.h>
#include <iostream>

namespace MagicPodsCore {

    class DBusService {
    private:
        std::unique_ptr<sdbus::IProxy> _rootProxy{};
        std::unique_ptr<sdbus::IProxy> _defaultBluetoothAdapterProxy{};

        std::map<sdbus::ObjectPath, std::shared_ptr<DBusDeviceInfo>> _knownDevices{};
        std::set<std::shared_ptr<DBusDeviceInfo>> _pairedDevices{};

        Event<std::shared_ptr<DBusDeviceInfo>> _onDeviceAddedEvent{};
        Event<std::shared_ptr<DBusDeviceInfo>> _onAnyDeviceAddedEvent{};
        Event<std::shared_ptr<DBusDeviceInfo>> _onDeviceRemovedEvent{};

        ObservableVariable<bool> _isBluetoothAdapterPowered{false};

    public:
        explicit DBusService();

        std::set<std::shared_ptr<DBusDeviceInfo>> GetAllDevices();
        std::set<std::shared_ptr<DBusDeviceInfo>> GetPairedDevices();

        ObservableVariable<bool>& IsBluetoothAdapterPowered() {
            return _isBluetoothAdapterPowered;
        }

        void EnableBluetoothAdapter();
        void EnableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback);
        void DisableBluetoothAdapter();
        void DisableBluetoothAdapterAsync(std::function<void(const sdbus::Error*)>&& callback);

        void SetDiscoveryFilter(const std::map<std::string, sdbus::Variant>& filter);
        void SetDiscoveryFilterAsync(const std::map<std::string, sdbus::Variant>& filter, std::function<void(const sdbus::Error*)>&& callback);

        void StartDiscovery();
        void StartDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback);
        void StopDiscovery();
        void StopDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback);

        Event<std::shared_ptr<DBusDeviceInfo>>& GetOnDeviceAddedEvent()
        {
            return _onDeviceAddedEvent;
        }

        Event<std::shared_ptr<DBusDeviceInfo>>& GetOnAnyDeviceAddedEvent()
        {
            return _onAnyDeviceAddedEvent;
        }

        Event<std::shared_ptr<DBusDeviceInfo>>& GetOnDeviceRemovedEvent()
        {
            return _onDeviceRemovedEvent;
        }

    private:
        void FetchDevices();

        std::shared_ptr<DBusDeviceInfo> TryCreateDevice(sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces);
        bool TryRemoveDevice(sdbus::ObjectPath objectPath);

        bool TryUpdateInterfaceAddedForDevice(sdbus::ObjectPath objectPath, std::map<std::string, std::map<std::string, sdbus::Variant>> interfaces);
        bool TryUpdateInterfaceRemovedForDevice(sdbus::ObjectPath objectPath);
    };

}