// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "ObservableVariable.h"

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <sdbus-c++/sdbus-c++.h>

namespace MagicPodsCore {

    // Keeps track of BlueZ adapters and sends requests to the selected adapter.
    // The first available adapter stays selected until BlueZ removes it.
    class DBusBtAdapter {
    private:
        // D-Bus signal callbacks and application requests run on different threads.
        mutable std::mutex _mutex{};
        ObservableVariable<bool> _isPowered{false};
        std::set<sdbus::ObjectPath> _availableAdapterPaths{};
        sdbus::ObjectPath _currentAdapterPath{};

        // Shared ownership lets an in-progress request finish while BlueZ
        // replaces the adapter.
        std::shared_ptr<sdbus::IProxy> _currentAdapterProxy{};

    public:
        DBusBtAdapter() = default;
        ~DBusBtAdapter();

        DBusBtAdapter(const DBusBtAdapter&) = delete;
        DBusBtAdapter(DBusBtAdapter&&) noexcept = delete;
        DBusBtAdapter& operator=(const DBusBtAdapter&) = delete;
        DBusBtAdapter& operator=(DBusBtAdapter&&) noexcept = delete;

        void AddAdapter(const sdbus::ObjectPath& objectPath);
        void RemoveAdapter(const sdbus::ObjectPath& objectPath);

        ObservableVariable<bool>& IsPowered() {
            return _isPowered;
        }

        void SetPowered(bool powered);
        void SetPoweredAsync(bool powered, std::function<void(const sdbus::Error*)>&& callback);

        void SetDiscoveryFilter(const std::map<std::string, sdbus::Variant>& filter);
        void SetDiscoveryFilterAsync(const std::map<std::string, sdbus::Variant>& filter,
                                     std::function<void(const sdbus::Error*)>&& callback);

        void StartDiscovery();
        void StartDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback);
        void StopDiscovery();
        void StopDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback);

    private:
        std::shared_ptr<sdbus::IProxy> GetCurrentAdapterProxy() const;
        bool IsCurrentAdapter(const sdbus::ObjectPath& objectPath) const;
        void SelectAdapter(const sdbus::ObjectPath& objectPath);
        void HandlePropertiesChanged(const sdbus::ObjectPath& objectPath,
                                     const std::string& interfaceName,
                                     const std::map<std::string, sdbus::Variant>& values);
    };

}
