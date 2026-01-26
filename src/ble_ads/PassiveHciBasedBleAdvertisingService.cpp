// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PassiveHciBasedBleAdvertisingService.h"
#include <iostream>

namespace MagicPodsCore
{

PassiveHciBasedBleAdvertisingService::PassiveHciBasedBleAdvertisingService(DBusService& dbusService, int deviceId)
    : _dbusService(dbusService), _deviceId(deviceId)
{
    // Подписываемся на изменения состояния Bluetooth адаптера
    _adapterPoweredSubscriptionId = _dbusService.IsBluetoothAdapterPowered().GetEvent().Subscribe(
        [this](size_t listenerId, bool isPowered) {
            this->OnAdapterPowerChanged(listenerId, isPowered);
        }
    );
}

PassiveHciBasedBleAdvertisingService::~PassiveHciBasedBleAdvertisingService()
{
    // Отписываемся от события изменения состояния адаптера
    if (_adapterPoweredSubscriptionId != 0) {
        _dbusService.IsBluetoothAdapterPowered().GetEvent().Unsubscribe(_adapterPoweredSubscriptionId);
        _adapterPoweredSubscriptionId = 0;
    }

    StopScan();
}

std::map<uint16_t, std::vector<uint8_t>> PassiveHciBasedBleAdvertisingService::ParseManufacturerData(const uint8_t* data, uint8_t len)
{
    std::map<uint16_t, std::vector<uint8_t>> result;
    uint8_t pos = 0;

    while (pos < len)
    {
        uint8_t field_len = data[pos++];
        if (field_len == 0 || pos >= len)
            break;

        uint8_t field_type = data[pos++];
        const uint8_t* field_data = &data[pos];
        uint8_t data_len = field_len - 1;

        // 0xFF = Manufacturer Specific Data
        if (field_type == 0xFF && data_len >= 2)
        {
            uint16_t company_id = field_data[0] | (field_data[1] << 8);
            result[company_id] = std::vector<uint8_t>(field_data + 2, field_data + data_len);
        }

        pos += data_len;
    }

    return result;
}

void PassiveHciBasedBleAdvertisingService::ParseExtendedAdvertisingReport(const uint8_t* data)
{
    uint8_t num_reports = data[0];
    const uint8_t* ptr = data + 1;

    for (int i = 0; i < num_reports; i++)
    {
        le_ext_advertising_info* info = (le_ext_advertising_info*)ptr;

        // Извлекаем MAC адрес
        char addr[18];
        ba2str(&info->bdaddr, addr);
        std::string address(addr);

        // RSSI
        int8_t rssi = info->rssi;

        // Длина advertising данных
        uint8_t adv_len = info->length;

        // Парсим manufacturer data
        auto manufacturerData = ParseManufacturerData(info->data, adv_len);

        // Отправляем событие только если есть manufacturer data
        if (!manufacturerData.empty())
        {
            _onAdReceivedEvent.FireEvent(BleAdertisingData(address, rssi, manufacturerData));
        }

        // Переходим к следующему отчету
        ptr += sizeof(le_ext_advertising_info) + adv_len;
    }
}

void PassiveHciBasedBleAdvertisingService::ParseAdvertisingReport(const uint8_t* data)
{
    uint8_t num_reports = data[0];
    const uint8_t* ptr = data + 1;

    for (int i = 0; i < num_reports; i++)
    {
        le_advertising_info* info = (le_advertising_info*)ptr;

        // Извлекаем MAC адрес
        char addr[18];
        ba2str(&info->bdaddr, addr);
        std::string address(addr);

        // Длина advertising данных
        uint8_t adv_len = info->length;

        // RSSI находится после advertising данных
        int8_t rssi = (int8_t)info->data[adv_len];

        // Парсим manufacturer data
        auto manufacturerData = ParseManufacturerData(info->data, adv_len);

        // Отправляем событие только если есть manufacturer data
        if (!manufacturerData.empty())
        {
            _onAdReceivedEvent.FireEvent(BleAdertisingData(address, rssi, manufacturerData));
        }

        // Переходим к следующему отчету
        ptr += sizeof(le_advertising_info) + adv_len + 1;
    }
}

void PassiveHciBasedBleAdvertisingService::ProcessEvent(const unsigned char* buf, ssize_t len)
{
    if (len < (1 + HCI_EVENT_HDR_SIZE))
        return;

    hci_event_hdr* hdr = (hci_event_hdr*)(buf + 1);

    if (hdr->evt != EVT_LE_META_EVENT)
        return;

    evt_le_meta_event* meta = (evt_le_meta_event*)(buf + 1 + HCI_EVENT_HDR_SIZE);

    // Поддерживаем как обычные, так и расширенные advertising reports (BT 5.0+)
    if (meta->subevent == EVT_LE_ADVERTISING_REPORT)
    {
        ParseAdvertisingReport((uint8_t*)meta->data);
    }
    else if (meta->subevent == EVT_LE_EXT_ADVERTISING_REPORT)
    {
        ParseExtendedAdvertisingReport((uint8_t*)meta->data);
    }
}

void PassiveHciBasedBleAdvertisingService::ListenLoop()
{
    unsigned char buf[HCI_MAX_EVENT_SIZE];

    while (_isRunning)
    {
        // Используем poll() с таймаутом вместо блокирующего read()
        // Это позволяет периодически проверять _isRunning и корректно завершаться
        struct pollfd pfd;
        pfd.fd = _hciSocket;
        pfd.events = POLLIN;

        int ret = poll(&pfd, 1, 100); // 100ms timeout

        if (ret < 0)
        {
            if (errno == EINTR)
                continue;
            break;
        }

        if (ret == 0)
        {
            // Timeout - проверяем _isRunning и продолжаем
            continue;
        }

        // Данные доступны для чтения
        ssize_t len = read(_hciSocket, buf, sizeof(buf));

        if (len < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            // Ошибка чтения - выходим из цикла
            break;
        }

        if (len == 0)
        {
            // EOF - сокет закрыт
            break;
        }

        if (len > 0)
        {
            ProcessEvent(buf, len);
        }
    }
}

bool PassiveHciBasedBleAdvertisingService::OpenHciDevice()
{
    _hciSocket = hci_open_dev(_deviceId);
    if (_hciSocket < 0)
        return false;

    // Сохраняем старый фильтр
    socklen_t olen = sizeof(_oldFilter);
    if (getsockopt(_hciSocket, SOL_HCI, HCI_FILTER, &_oldFilter, &olen) < 0)
    {
        CloseHciDevice();
        return false;
    }

    // Устанавливаем фильтр для получения только LE events
    struct hci_filter nf;
    hci_filter_clear(&nf);
    hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
    hci_filter_set_event(EVT_LE_META_EVENT, &nf);

    if (setsockopt(_hciSocket, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
    {
        CloseHciDevice();
        return false;
    }

    return true;
}

void PassiveHciBasedBleAdvertisingService::CloseHciDevice()
{
    if (_hciSocket >= 0)
    {
        // Восстанавливаем старый фильтр
        setsockopt(_hciSocket, SOL_HCI, HCI_FILTER, &_oldFilter, sizeof(_oldFilter));

        // Отключаем сокет для чтения, чтобы разблокировать read() в другом потоке
        shutdown(_hciSocket, SHUT_RDWR);

        hci_close_dev(_hciSocket);
        _hciSocket = -1;
    }
}

void PassiveHciBasedBleAdvertisingService::OnAdapterPowerChanged(size_t listenerId, bool isPowered)
{
    if (!isPowered)
    {
        std::cout << "[PassiveHCI] Bluetooth adapter powered OFF, scanning stopped" << std::endl;
        // Сканирование остановится автоматически (HCI сокет закроется)
        // Флаг _scanDesired остается true для последующего восстановления
    }
    else if (_scanDesired)
    {
        std::cout << "[PassiveHCI] Bluetooth adapter powered ON, restarting scan..." << std::endl;

        // ВАЖНО: Запускаем перезапуск в отдельном потоке, чтобы не блокировать
        // DBus event loop синхронными вызовами StartDiscovery()
        std::thread([this]() {
            try {
                // Небольшая задержка для стабилизации адаптера
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Перезапускаем сканирование
                StartScan(false);
                StartListening();
                std::cout << "[PassiveHCI] Scan successfully restarted" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[PassiveHCI] Failed to restart scan: " << e.what() << std::endl;
                // Следующее включение адаптера вызовет новую попытку восстановления
            }
        }).detach();
    }
}

void PassiveHciBasedBleAdvertisingService::StartScan(bool isPassive)
{
    // 1. Используем DBus для БЕЗОПАСНОГО управления сканированием
    //    Это кооперативно с другими процессами (bluetoothd, другие приложения)
    std::map<std::string, sdbus::Variant> filter;
    filter.emplace("Transport", sdbus::Variant("le"));
    filter.emplace("DuplicateData", sdbus::Variant(true));

    // Пытаемся установить фильтр (игнорируем ошибки, фильтр может быть уже установлен)
    try {
        _dbusService.SetDiscoveryFilter(filter);
    } catch (const sdbus::Error& e) {
        // Игнорируем - фильтр может быть уже установлен другим процессом
        std::cout << "Note: Could not set discovery filter (may already be set): "
                  << e.getMessage() << std::endl;
    }

    // Пытаемся запустить discovery (игнорируем ошибки "InProgress" и "NotReady")
    try {
        _dbusService.StartDiscovery();  // Кооперативное управление
    } catch (const sdbus::Error& e) {
        std::string errorName = e.getName();
        if (errorName.find("InProgress") != std::string::npos) {
            // Это нормально! Другой процесс уже запустил сканирование
            // Мы просто присоединимся к нему через HCI сокет
            std::cout << "Note: Discovery already in progress (joining existing scan)" << std::endl;
        } else if (errorName.find("NotReady") != std::string::npos) {
            // Bluetooth адаптер выключен - не запускаем сканирование
            // Когда адаптер включится, OnAdapterPowerChanged() автоматически перезапустит сканирование
            std::cerr << "Warning: Bluetooth adapter is powered OFF. Waiting for adapter to be enabled..." << std::endl;
            _scanStartedSuccessfully = false;
            _scanDesired = true;  // Устанавливаем флаг, чтобы OnAdapterPowerChanged() перезапустил сканирование
            return;
        } else {
            // Другая неожиданная ошибка - пробрасываем дальше
            throw;
        }
    }

    // 2. Ждем пока bluetoothd активирует сканирование на адаптере
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 3. Открываем HCI сокет для ПАССИВНОГО чтения событий
    //    ВАЖНО: Мы НЕ вызываем hci_le_set_scan_enable!
    //    Просто читаем события, которые генерирует bluetoothd
    if (!OpenHciDevice())
    {
        // Пытаемся остановить discovery (если мы его запустили)
        try {
            _dbusService.StopDiscovery();
        } catch (...) {
            // Игнорируем ошибки при остановке
        }
        _scanStartedSuccessfully = false;
        return;
    }

    // Готово! bluetoothd управляет сканированием, мы только читаем события
    _scanStartedSuccessfully = true;
    _scanDesired = true;  // Запоминаем что сканирование должно быть активно
}

void PassiveHciBasedBleAdvertisingService::StopScan()
{
    _scanDesired = false;  // Сбрасываем флаг, чтобы не восстанавливать при повторном включении

    // Останавливаем поток прослушивания
    if (_isRunning)
    {
        _isRunning = false;

        // КРИТИЧЕСКИ ВАЖНО: Закрываем сокет ДО join(), чтобы разблокировать poll()/read()
        CloseHciDevice();

        if (_listenThread.joinable())
            _listenThread.join();
    }
    else
    {
        // Если поток не запущен, всё равно закрываем сокет (если открыт)
        CloseHciDevice();
    }

    // Останавливаем DBus discovery только если сканирование было успешно запущено
    // Это предотвращает блокировку при попытке остановить незапущенное сканирование
    if (_scanStartedSuccessfully)
    {
        // Используем асинхронный вызов для предотвращения зависания
        // при переподключении адаптера или таймауте DBus
        _dbusService.StopDiscoveryAsync(
            [](const sdbus::Error* error) {
                if (error) {
                    // Игнорируем - другие процессы могут продолжать использовать сканирование
                    std::cout << "Note: Could not stop discovery (may be used by other processes): "
                              << error->getMessage() << std::endl;
                }
            }
        );
        _scanStartedSuccessfully = false;
    }
}

void PassiveHciBasedBleAdvertisingService::StartListening()
{
    if (_hciSocket < 0 || _isRunning)
        return;

    _isRunning = true;
    _listenThread = std::thread(&PassiveHciBasedBleAdvertisingService::ListenLoop, this);
}

}
