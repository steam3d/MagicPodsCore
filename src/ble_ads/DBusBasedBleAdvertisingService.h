// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "BleAdvertisingService.h"
#include "../dbus/DBusService.h"
#include "BleAdverisingData.h"
#include <memory>
#include <atomic>

namespace MagicPodsCore
{

class DBusBasedBleAdvertisingService : public BleAdvertisingService
{
private:
    DBusService& _dbusService;
    size_t _onDeviceAddedSubscriptionId = 0;
    size_t _adapterPoweredSubscriptionId = 0;
    std::atomic<bool> _scanDesired{false};

    void OnDeviceAdded(std::shared_ptr<DBusDeviceInfo> deviceInfo);
    void OnAdapterPowerChanged(size_t listenerId, bool isPowered);

public:
    explicit DBusBasedBleAdvertisingService(DBusService& dbusService);
    ~DBusBasedBleAdvertisingService() override;

    void StartScan(bool isPassive) override;
    void StopScan() override;
    void StartListening() override;
};

}