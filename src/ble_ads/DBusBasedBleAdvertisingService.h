// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "BleAdvertisingService.h"
#include "../dbus/DBusService.h"
#include "BleAdverisingData.h"
#include <memory>
#include <atomic>
#include <map>
#include <mutex>

namespace MagicPodsCore
{

class DBusBasedBleAdvertisingService : public BleAdvertisingService
{
private:
    struct DeviceSubscriptions {
        std::weak_ptr<DBusDeviceInfo> device;
        size_t manufacturerDataSubscriptionId = 0;
    };

    DBusService& _dbusService;
    size_t _onDeviceAddedSubscriptionId = 0;
    size_t _onDeviceRemovedSubscriptionId = 0;
    size_t _adapterPoweredSubscriptionId = 0;
    std::atomic<bool> _scanDesired{false};
    std::map<std::string, DeviceSubscriptions> _deviceSubscriptions{};
    std::mutex _deviceSubscriptionsMutex{};

    void OnDeviceAdded(std::shared_ptr<DBusDeviceInfo> deviceInfo);
    void OnAdapterPowerChanged(size_t listenerId, bool isPowered);
    void EmitAd(const std::shared_ptr<DBusDeviceInfo>& deviceInfo,
                const std::map<uint16_t, std::vector<uint8_t>>& manufacturerData);
    void RemoveDeviceSubscriptions(const std::string& address);

public:
    explicit DBusBasedBleAdvertisingService(DBusService& dbusService);
    ~DBusBasedBleAdvertisingService() override;

    void StartScan(bool isPassive) override;
    void StopScan() override;
    void StartListening() override;
};

}
