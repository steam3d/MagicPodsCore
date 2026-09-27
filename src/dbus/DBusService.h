// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "DBusBtAdapter.h"
#include "DBusDeviceInfo.h"
#include "ObservableVariable.h"

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include <sdbus-c++/sdbus-c++.h>

namespace MagicPodsCore {

    class DBusService {
    private:
        using InterfaceMap = std::map<std::string, std::map<std::string, sdbus::Variant>>;

        std::map<sdbus::ObjectPath, std::shared_ptr<DBusDeviceInfo>> _knownDevices{};
        std::set<std::shared_ptr<DBusDeviceInfo>> _pairedDevices{};

        Event<std::shared_ptr<DBusDeviceInfo>> _onDeviceAddedEvent{};
        Event<std::shared_ptr<DBusDeviceInfo>> _onAnyDeviceAddedEvent{};
        Event<std::shared_ptr<DBusDeviceInfo>> _onDeviceRemovedEvent{};

        DBusBtAdapter _bluetoothAdapter{};
        std::unique_ptr<sdbus::IProxy> _rootProxy{};

    public:
        explicit DBusService();
        ~DBusService();

        DBusService(const DBusService&) = delete;
        DBusService(DBusService&&) noexcept = delete;
        DBusService& operator=(const DBusService&) = delete;
        DBusService& operator=(DBusService&&) noexcept = delete;

        std::set<std::shared_ptr<DBusDeviceInfo>> GetAllDevices();
        std::set<std::shared_ptr<DBusDeviceInfo>> GetPairedDevices();

        ObservableVariable<bool>& IsBluetoothAdapterPowered() {
            return _bluetoothAdapter.IsPowered();
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
        void HandleInterfacesAdded(const sdbus::ObjectPath& objectPath, const InterfaceMap& interfaces);
        void HandleInterfacesRemoved(const sdbus::ObjectPath& objectPath, const std::vector<std::string>& interfaces);

        std::shared_ptr<DBusDeviceInfo> TryCreateDevice(const sdbus::ObjectPath& objectPath,
                                                        const InterfaceMap& interfaces);
        bool TryRemoveDevice(const sdbus::ObjectPath& objectPath);

        bool TryUpdateInterfaceAddedForDevice(const sdbus::ObjectPath& objectPath,
                                              const InterfaceMap& interfaces);
        bool TryUpdateInterfaceRemovedForDevice(const sdbus::ObjectPath& objectPath);
    };

}
