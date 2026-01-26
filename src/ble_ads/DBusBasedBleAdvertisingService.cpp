// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "DBusBasedBleAdvertisingService.h"
#include <iostream>

namespace MagicPodsCore
{

DBusBasedBleAdvertisingService::DBusBasedBleAdvertisingService(DBusService& dbusService)
    : _dbusService(dbusService)
{
    // Подписываемся на изменения состояния Bluetooth адаптера
    _adapterPoweredSubscriptionId = _dbusService.IsBluetoothAdapterPowered().GetEvent().Subscribe(
        [this](size_t listenerId, bool isPowered) {
            this->OnAdapterPowerChanged(listenerId, isPowered);
        }
    );
}

DBusBasedBleAdvertisingService::~DBusBasedBleAdvertisingService()
{
    // Отписываемся от события изменения состояния адаптера
    if (_adapterPoweredSubscriptionId != 0) {
        _dbusService.IsBluetoothAdapterPowered().GetEvent().Unsubscribe(_adapterPoweredSubscriptionId);
        _adapterPoweredSubscriptionId = 0;
    }

    // Сначала отписываемся от событий
    if (_onDeviceAddedSubscriptionId != 0) {
        _dbusService.GetOnAnyDeviceAddedEvent().Unsubscribe(_onDeviceAddedSubscriptionId);
        _onDeviceAddedSubscriptionId = 0; // Сбрасываем ID после отписки
    }
    if (_onDeviceRemovedSubscriptionId != 0) {
        _dbusService.GetOnDeviceRemovedEvent().Unsubscribe(_onDeviceRemovedSubscriptionId);
        _onDeviceRemovedSubscriptionId = 0;
    }
    {
        std::lock_guard<std::mutex> lock(_deviceSubscriptionsMutex);
        for (auto& [address, subs] : _deviceSubscriptions) {
            if (auto device = subs.device.lock()) {
                if (subs.manufacturerDataSubscriptionId != 0) {
                    device->GetManufacturerData().GetEvent().Unsubscribe(subs.manufacturerDataSubscriptionId);
                }
            }
        }
        _deviceSubscriptions.clear();
    }

    // Вызываем StopDiscovery как меру предосторожности.
    // ВНИМАНИЕ: Это предположение, что _dbusService (экземпляр DBusService)
    // будет все еще валиден и жив на момент вызова этого деструктора.
    // Если _dbusService уничтожается раньше этого объекта, этот вызов
    // может привести к неопределенному поведению (use-after-free).
    try {
        _dbusService.StopDiscovery();
    } catch (const sdbus::Error& e) {
        // Игнорируем ошибки в деструкторе
    }
}

void DBusBasedBleAdvertisingService::OnDeviceAdded(std::shared_ptr<DBusDeviceInfo> deviceInfo)
{
    std::string address = deviceInfo->GetAddress();
    RemoveDeviceSubscriptions(address);

    EmitAd(deviceInfo, deviceInfo->GetManufacturerData().GetValue());

    DeviceSubscriptions subs{};
    subs.device = deviceInfo;
    subs.manufacturerDataSubscriptionId =
        deviceInfo->GetManufacturerData().GetEvent().Subscribe(
            [this, deviceInfo](size_t, const std::map<uint16_t, std::vector<uint8_t>>& data) {
                EmitAd(deviceInfo, data);
            });

    {
        std::lock_guard<std::mutex> lock(_deviceSubscriptionsMutex);
        _deviceSubscriptions[address] = subs;
    }
}

void DBusBasedBleAdvertisingService::OnAdapterPowerChanged(size_t listenerId, bool isPowered)
{
    if (!isPowered) {
        std::cout << "[DBus] Bluetooth adapter powered OFF, scanning stopped" << std::endl;
        // Сканирование остановится автоматически (DBus events прекратятся)
        // Флаг _scanDesired остается true для последующего восстановления
    } else if (_scanDesired) {
        std::cout << "[DBus] Bluetooth adapter powered ON, restarting scan..." << std::endl;

        // ВАЖНО: Запускаем перезапуск в отдельном потоке, чтобы не блокировать
        // DBus event loop синхронными вызовами StartDiscovery()
        std::thread([this]() {
            try {
                // Небольшая задержка для стабилизации адаптера
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Перезапускаем сканирование
                StartListening();
                StartScan(false);
                std::cout << "[DBus] Scan successfully restarted" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[DBus] Failed to restart scan: " << e.what() << std::endl;
                // Следующее включение адаптера вызовет новую попытку восстановления
            }
        }).detach();
    }
}

void DBusBasedBleAdvertisingService::StartScan(bool isPassive)
{
    std::map<std::string, sdbus::Variant> filter;
    filter.emplace("Transport", sdbus::Variant("le"));
    filter.emplace("DuplicateData", sdbus::Variant(true)); // Add this line to receive duplicate data
    // The isPassive flag is not directly supported by the SetDiscoveryFilter method.
    // The bluetoothd daemon manages the scanning mode automatically based on the needs of all clients.

    // Пытаемся установить фильтр (игнорируем ошибки, фильтр может быть уже установлен)
    try {
        _dbusService.SetDiscoveryFilter(filter);
    } catch (const sdbus::Error& e) {
        std::cout << "Note: Could not set discovery filter (may already be set): "
                  << e.getMessage() << std::endl;
    }

    // Пытаемся запустить discovery (игнорируем ошибки "InProgress" и "NotReady")
    try {
        _dbusService.StartDiscovery();
    } catch (const sdbus::Error& e) {
        std::string errorName = e.getName();
        if (errorName.find("InProgress") != std::string::npos) {
            std::cout << "Note: Discovery already in progress (joining existing scan)" << std::endl;
        } else if (errorName.find("NotReady") != std::string::npos) {
            // Bluetooth адаптер выключен - не запускаем сканирование
            // Когда адаптер включится, OnAdapterPowerChanged() автоматически перезапустит сканирование
            std::cerr << "Warning: Bluetooth adapter is powered OFF. Waiting for adapter to be enabled..." << std::endl;
            _scanDesired = true;  // Устанавливаем флаг, чтобы OnAdapterPowerChanged() перезапустил сканирование
            return;
        } else {
            throw;  // Пробрасываем другие неожиданные ошибки
        }
    }

    _scanDesired = true;  // Запоминаем что сканирование должно быть активно
}

void DBusBasedBleAdvertisingService::StopScan()
{
    _scanDesired = false;  // Сбрасываем флаг, чтобы не восстанавливать при повторном включении

    if (_onDeviceAddedSubscriptionId != 0) {
        _dbusService.GetOnAnyDeviceAddedEvent().Unsubscribe(_onDeviceAddedSubscriptionId);
        _onDeviceAddedSubscriptionId = 0;
    }
    if (_onDeviceRemovedSubscriptionId != 0) {
        _dbusService.GetOnDeviceRemovedEvent().Unsubscribe(_onDeviceRemovedSubscriptionId);
        _onDeviceRemovedSubscriptionId = 0;
    }
    {
        std::lock_guard<std::mutex> lock(_deviceSubscriptionsMutex);
        for (auto& [address, subs] : _deviceSubscriptions) {
            if (auto device = subs.device.lock()) {
                if (subs.manufacturerDataSubscriptionId != 0) {
                    device->GetManufacturerData().GetEvent().Unsubscribe(subs.manufacturerDataSubscriptionId);
                }
            }
        }
        _deviceSubscriptions.clear();
    }

    // Используем асинхронный вызов для предотвращения зависания
    // при переподключении адаптера или таймауте DBus
    _dbusService.StopDiscoveryAsync(
        [](const sdbus::Error* error) {
            if (error) {
                std::cout << "Note: Could not stop discovery (may be used by other processes): "
                          << error->getMessage() << std::endl;
            }
        }
    );
}

void DBusBasedBleAdvertisingService::StartListening()
{
    if (_onDeviceAddedSubscriptionId != 0) {
        _dbusService.GetOnAnyDeviceAddedEvent().Unsubscribe(_onDeviceAddedSubscriptionId);
    }
    if (_onDeviceRemovedSubscriptionId != 0) {
        _dbusService.GetOnDeviceRemovedEvent().Unsubscribe(_onDeviceRemovedSubscriptionId);
    }
    _onDeviceAddedSubscriptionId = _dbusService.GetOnAnyDeviceAddedEvent().Subscribe(
        [this](size_t, std::shared_ptr<DBusDeviceInfo> dev) {
            this->OnDeviceAdded(dev);
        });
    _onDeviceRemovedSubscriptionId = _dbusService.GetOnDeviceRemovedEvent().Subscribe(
        [this](size_t, std::shared_ptr<DBusDeviceInfo> dev) {
            RemoveDeviceSubscriptions(dev->GetAddress());
        });
}

void DBusBasedBleAdvertisingService::EmitAd(
    const std::shared_ptr<DBusDeviceInfo>& deviceInfo,
    const std::map<uint16_t, std::vector<uint8_t>>& manufacturerData)
{
    if (manufacturerData.empty()) {
        return;
    }

    std::string address = deviceInfo->GetAddress();
    auto rssi = static_cast<int8_t>(deviceInfo->GetRssi().GetValue());

    _onAdReceivedEvent.FireEvent(BleAdertisingData(address, rssi, manufacturerData));
}

void DBusBasedBleAdvertisingService::RemoveDeviceSubscriptions(const std::string& address)
{
    std::lock_guard<std::mutex> lock(_deviceSubscriptionsMutex);
    auto it = _deviceSubscriptions.find(address);
    if (it == _deviceSubscriptions.end()) {
        return;
    }
    auto device = it->second.device.lock();
    if (device) {
        if (it->second.manufacturerDataSubscriptionId != 0) {
            device->GetManufacturerData().GetEvent().Unsubscribe(it->second.manufacturerDataSubscriptionId);
        }
    }
    _deviceSubscriptions.erase(it);
}

}
