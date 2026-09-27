// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "DBusBtAdapter.h"
#include "Logger.h"

#include <utility>
#include <vector>

namespace MagicPodsCore {

    namespace {
        constexpr auto BLUEZ_ADAPTER_INTERFACE = "org.bluez.Adapter1";
        constexpr auto DBUS_PROPERTIES_INTERFACE = "org.freedesktop.DBus.Properties";

        sdbus::Error MakeNoBluetoothAdapterError() {
            return sdbus::Error{"org.bluez.Error.NotReady", "No Bluetooth adapter is available"};
        }
    }

    DBusBtAdapter::~DBusBtAdapter() {
        // Stop the signal callback before the fields used by it are destroyed.
        _currentAdapterProxy.reset();
    }

    void DBusBtAdapter::AddAdapter(const sdbus::ObjectPath& objectPath) {
        bool shouldSelect;
        {
            std::lock_guard lock{_mutex};
            _availableAdapterPaths.emplace(objectPath);
            shouldSelect = _currentAdapterPath.empty();
        }

        if (shouldSelect) {
            SelectAdapter(objectPath);
        }
    }

    void DBusBtAdapter::RemoveAdapter(const sdbus::ObjectPath& objectPath) {
        std::shared_ptr<sdbus::IProxy> removedProxy;
        sdbus::ObjectPath replacementPath;

        {
            std::lock_guard lock{_mutex};
            _availableAdapterPaths.erase(objectPath);

            if (_currentAdapterPath != objectPath) {
                return;
            }

            _currentAdapterPath.clear();
            removedProxy = std::move(_currentAdapterProxy);

            if (!_availableAdapterPaths.empty()) {
                replacementPath = *_availableAdapterPaths.begin();
            }
        }

        Logger::Info("Bluetooth adapter removed: %s", objectPath.c_str());

        // Wait until callbacks from the removed adapter have finished before
        // publishing its final powered-off state.
        removedProxy.reset();
        _isPowered.SetValue(false);

        if (!replacementPath.empty()) {
            SelectAdapter(replacementPath);
        }
    }

    void DBusBtAdapter::SelectAdapter(const sdbus::ObjectPath& objectPath) {
        std::shared_ptr<sdbus::IProxy> adapterProxy;

        try {
            adapterProxy = sdbus::createProxy("org.bluez", objectPath);
            adapterProxy->uponSignal("PropertiesChanged").onInterface(DBUS_PROPERTIES_INTERFACE).call(
                [this, objectPath](std::string interfaceName,
                                   std::map<std::string, sdbus::Variant> values,
                                   std::vector<std::string>) {
                    HandlePropertiesChanged(objectPath, interfaceName, values);
                });
            adapterProxy->finishRegistration();
        }
        catch (const sdbus::Error& error) {
            Logger::Error("Failed to initialize Bluetooth adapter %s: %s", objectPath.c_str(), error.what());
            return;
        }

        {
            std::lock_guard lock{_mutex};
            if (!_availableAdapterPaths.contains(objectPath) || !_currentAdapterPath.empty()) {
                return;
            }

            _currentAdapterPath = objectPath;
            _currentAdapterProxy = adapterProxy;
        }

        bool powered = false;
        try {
            powered = adapterProxy->getProperty("Powered").onInterface(BLUEZ_ADAPTER_INTERFACE).get<bool>();
        }
        catch (const sdbus::Error& error) {
            Logger::Error("Failed to read Powered from Bluetooth adapter %s: %s", objectPath.c_str(), error.what());
        }

        // The adapter may have disappeared while the D-Bus call was running.
        if (!IsCurrentAdapter(objectPath)) {
            return;
        }

        Logger::Info("Using Bluetooth adapter: %s", objectPath.c_str());
        _isPowered.SetValue(powered);
    }

    void DBusBtAdapter::HandlePropertiesChanged(
        const sdbus::ObjectPath& objectPath,
        const std::string& interfaceName,
        const std::map<std::string, sdbus::Variant>& values) {
        try {
            if (interfaceName != BLUEZ_ADAPTER_INTERFACE || !values.contains("Powered")) {
                return;
            }

            if (!IsCurrentAdapter(objectPath)) {
                return;
            }

            _isPowered.SetValue(values.at("Powered").get<bool>());
        }
        catch (const std::exception& error) {
            Logger::Error("Failed to handle adapter PropertiesChanged for %s: %s", objectPath.c_str(), error.what());
        }
    }

    std::shared_ptr<sdbus::IProxy> DBusBtAdapter::GetCurrentAdapterProxy() const {
        std::lock_guard lock{_mutex};
        return _currentAdapterProxy;
    }

    bool DBusBtAdapter::IsCurrentAdapter(const sdbus::ObjectPath& objectPath) const {
        std::lock_guard lock{_mutex};
        return _currentAdapterPath == objectPath;
    }

    void DBusBtAdapter::SetPowered(bool powered) {
        SetPoweredAsync(powered, [powered](const sdbus::Error* error) {
            if (error) {
                Logger::Error("Failed to %s Bluetooth adapter: %s", powered ? "enable" : "disable", error->what());
            }
        });
    }

    void DBusBtAdapter::SetPoweredAsync(bool powered, std::function<void(const sdbus::Error*)>&& callback) {
        const auto adapterProxy = GetCurrentAdapterProxy();
        if (!adapterProxy) {
            const auto error = MakeNoBluetoothAdapterError();
            if (callback) {
                callback(&error);
            }
            return;
        }

        try {
            adapterProxy->setPropertyAsync("Powered")
                .onInterface(BLUEZ_ADAPTER_INTERFACE)
                .toValue(powered)
                .uponReplyInvoke(callback);
        }
        catch (const sdbus::Error& error) {
            Logger::Error("Failed to change Bluetooth adapter power: %s", error.what());
            if (callback) {
                callback(&error);
            }
        }
    }

    void DBusBtAdapter::SetDiscoveryFilter(const std::map<std::string, sdbus::Variant>& filter) {
        const auto adapterProxy = GetCurrentAdapterProxy();
        if (adapterProxy) {
            adapterProxy->callMethod("SetDiscoveryFilter")
                .onInterface(BLUEZ_ADAPTER_INTERFACE)
                .withArguments(filter);
        }
    }

    void DBusBtAdapter::SetDiscoveryFilterAsync(
        const std::map<std::string, sdbus::Variant>& filter,
        std::function<void(const sdbus::Error*)>&& callback) {
        const auto adapterProxy = GetCurrentAdapterProxy();
        if (!adapterProxy) {
            const auto error = MakeNoBluetoothAdapterError();
            if (callback) {
                callback(&error);
            }
            return;
        }

        try {
            adapterProxy->callMethodAsync("SetDiscoveryFilter")
                .onInterface(BLUEZ_ADAPTER_INTERFACE)
                .withArguments(filter)
                .uponReplyInvoke(callback);
        }
        catch (const sdbus::Error& error) {
            Logger::Error("Failed to set Bluetooth discovery filter: %s", error.what());
            if (callback) {
                callback(&error);
            }
        }
    }

    void DBusBtAdapter::StartDiscovery() {
        const auto adapterProxy = GetCurrentAdapterProxy();
        if (adapterProxy) {
            adapterProxy->callMethod("StartDiscovery")
                .onInterface(BLUEZ_ADAPTER_INTERFACE);
        }
    }

    void DBusBtAdapter::StartDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback) {
        const auto adapterProxy = GetCurrentAdapterProxy();
        if (!adapterProxy) {
            const auto error = MakeNoBluetoothAdapterError();
            if (callback) {
                callback(&error);
            }
            return;
        }

        try {
            adapterProxy->callMethodAsync("StartDiscovery")
                .onInterface(BLUEZ_ADAPTER_INTERFACE)
                .uponReplyInvoke(callback);
        }
        catch (const sdbus::Error& error) {
            Logger::Error("Failed to start Bluetooth discovery: %s", error.what());
            if (callback) {
                callback(&error);
            }
        }
    }

    void DBusBtAdapter::StopDiscovery() {
        const auto adapterProxy = GetCurrentAdapterProxy();
        if (adapterProxy) {
            adapterProxy->callMethod("StopDiscovery")
                .onInterface(BLUEZ_ADAPTER_INTERFACE)
                .dontExpectReply();
        }
    }

    void DBusBtAdapter::StopDiscoveryAsync(std::function<void(const sdbus::Error*)>&& callback) {
        const auto adapterProxy = GetCurrentAdapterProxy();
        if (!adapterProxy) {
            const auto error = MakeNoBluetoothAdapterError();
            if (callback) {
                callback(&error);
            }
            return;
        }

        try {
            adapterProxy->callMethodAsync("StopDiscovery")
                .onInterface(BLUEZ_ADAPTER_INTERFACE)
                .uponReplyInvoke(callback);
        }
        catch (const sdbus::Error& error) {
            Logger::Error("Failed to stop Bluetooth discovery: %s", error.what());
            if (callback) {
                callback(&error);
            }
        }
    }

}
