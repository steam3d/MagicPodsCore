// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "BleAdvertisingService.h"
#include "../dbus/DBusService.h"
#include <thread>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <poll.h>

// Bluetooth HCI headers
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

// Extended advertising report (BT 5.0+) - не определён в старых версиях hci.h
#ifndef EVT_LE_EXT_ADVERTISING_REPORT
#define EVT_LE_EXT_ADVERTISING_REPORT 0x0D
#endif

// Extended advertising info structure (BT 5.0)
typedef struct {
    uint16_t evt_type;
    uint8_t  bdaddr_type;
    bdaddr_t bdaddr;
    uint8_t  primary_phy;
    uint8_t  secondary_phy;
    uint8_t  advertising_sid;
    int8_t   tx_power;
    int8_t   rssi;
    uint16_t periodic_adv_int;
    uint8_t  direct_addr_type;
    bdaddr_t direct_addr;
    uint8_t  length;
    uint8_t  data[0];
} __attribute__ ((packed)) le_ext_advertising_info;

namespace MagicPodsCore
{

/**
 * Passive HCI-based BLE Advertising Service
 *
 * БЕЗОПАСНЫЙ ГИБРИДНЫЙ ПОДХОД:
 * - Использует DBus для управления сканированием (кооперативно с другими процессами)
 * - Читает события напрямую через HCI сокет (низкая задержка <10ms)
 * - НЕ вызывает hci_le_set_scan_enable (не конфликтует с bluetoothd)
 *
 * Преимущества:
 * - Работает параллельно с другими Bluetooth приложениями
 * - Низкая задержка получения advertising пакетов (<10ms)
 * - Безопасно для производственного использования
 * - Множественные экземпляры могут работать одновременно
 *
 * Как работает:
 * 1. DBus (bluetoothd) управляет сканированием через StartDiscovery()
 * 2. HCI сокет открывается только для ЧТЕНИЯ событий
 * 3. Ядро Linux дублирует HCI события всем открытым HCI сокетам
 * 4. Каждый сокет получает свою копию событий через HCI_FILTER
 *
 * Требования:
 * - Root права для открытия HCI сокета (один раз при StartScan)
 * - bluetoothd должен быть запущен и работать
 *
 * Сравнение с другими подходами:
 * - vs DBusBasedBleAdvertisingService: Быстрее (~10ms vs ~50-100ms)
 * - vs HciBasedBleAdvertisingService: Безопаснее (не ломает другие процессы)
 */
class PassiveHciBasedBleAdvertisingService : public BleAdvertisingService
{
private:
    DBusService& _dbusService;
    int _deviceId{0};
    int _hciSocket{-1};
    std::atomic<bool> _isRunning{false};
    std::atomic<bool> _scanStartedSuccessfully{false};
    std::thread _listenThread;
    struct hci_filter _oldFilter{};
    size_t _adapterPoweredSubscriptionId{0};
    std::atomic<bool> _scanDesired{false};

    std::map<uint16_t, std::vector<uint8_t>> ParseManufacturerData(const uint8_t* data, uint8_t len);
    void ParseExtendedAdvertisingReport(const uint8_t* data);
    void ParseAdvertisingReport(const uint8_t* data);
    void ProcessEvent(const unsigned char* buf, ssize_t len);
    void ListenLoop();
    bool OpenHciDevice();
    void CloseHciDevice();
    void OnAdapterPowerChanged(size_t listenerId, bool isPowered);

public:
    explicit PassiveHciBasedBleAdvertisingService(DBusService& dbusService, int deviceId = 0);
    ~PassiveHciBasedBleAdvertisingService() override;

    void StartScan(bool isPassive) override;
    void StopScan() override;
    void StartListening() override;
};

}