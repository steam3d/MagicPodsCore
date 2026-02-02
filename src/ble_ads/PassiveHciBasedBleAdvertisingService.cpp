// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PassiveHciBasedBleAdvertisingService.h"
#include <iostream>

namespace MagicPodsCore
{

PassiveHciBasedBleAdvertisingService::PassiveHciBasedBleAdvertisingService(DBusService& dbusService, int deviceId)
    : _dbusService(dbusService), _deviceId(deviceId)
{
    _adapterPoweredSubscriptionId = _dbusService.IsBluetoothAdapterPowered().GetEvent().Subscribe(
        [this](size_t listenerId, bool isPowered) {
            this->OnAdapterPowerChanged(listenerId, isPowered);
        }
    );
}

PassiveHciBasedBleAdvertisingService::~PassiveHciBasedBleAdvertisingService()
{
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

        char addr[18];
        ba2str(&info->bdaddr, addr);
        std::string address(addr);

        int8_t rssi = info->rssi;

        uint8_t adv_len = info->length;

        auto manufacturerData = ParseManufacturerData(info->data, adv_len);

        if (!manufacturerData.empty())
        {
            _onAdReceivedEvent.FireEvent(BleAdertisingData(address, rssi, manufacturerData));
        }

        // Go to next data
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

        char addr[18];
        ba2str(&info->bdaddr, addr);
        std::string address(addr);

        uint8_t adv_len = info->length;

        int8_t rssi = (int8_t)info->data[adv_len];

        auto manufacturerData = ParseManufacturerData(info->data, adv_len);

        if (!manufacturerData.empty())
        {
            _onAdReceivedEvent.FireEvent(BleAdertisingData(address, rssi, manufacturerData));
        }

        // Go to next data
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
        // Use poll() with a timeout instead of a blocking read().
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
            continue;
        }

        ssize_t len = read(_hciSocket, buf, sizeof(buf));

        if (len < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            break;
        }

        if (len == 0)
        {
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

    socklen_t olen = sizeof(_oldFilter);
    if (getsockopt(_hciSocket, SOL_HCI, HCI_FILTER, &_oldFilter, &olen) < 0)
    {
        CloseHciDevice();
        return false;
    }

    // Listen only LE events
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
        setsockopt(_hciSocket, SOL_HCI, HCI_FILTER, &_oldFilter, sizeof(_oldFilter));
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
    }
    else if (_scanDesired)
    {
        std::cout << "[PassiveHCI] Bluetooth adapter powered ON, restarting scan..." << std::endl;
        std::thread([this]() {
            try {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                StartScan(false);
                StartListening();
                std::cout << "[PassiveHCI] Scan successfully restarted" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[PassiveHCI] Failed to restart scan: " << e.what() << std::endl;
            }
        }).detach();
    }
}

void PassiveHciBasedBleAdvertisingService::StartScan(bool isPassive)
{
    std::map<std::string, sdbus::Variant> filter;
    filter.emplace("Transport", sdbus::Variant("le"));
    filter.emplace("DuplicateData", sdbus::Variant(true));

    try {
        _dbusService.SetDiscoveryFilter(filter);
    } catch (const sdbus::Error& e) {
        std::cout << "Note: Could not set discovery filter (may already be set): "
                  << e.getMessage() << std::endl;
    }

    try {
        _dbusService.StartDiscovery();
    } catch (const sdbus::Error& e) {
        std::string errorName = e.getName();
        if (errorName.find("InProgress") != std::string::npos) {
            std::cout << "Note: Discovery already in progress (joining existing scan)" << std::endl;
        } else if (errorName.find("NotReady") != std::string::npos) {
            std::cerr << "Warning: Bluetooth adapter is powered OFF. Waiting for adapter to be enabled..." << std::endl;
            _scanStartedSuccessfully = false;
            _scanDesired = true;
            return;
        } else {
            throw;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    //Do not call hci_le_set_scan_enable!
    if (!OpenHciDevice())
    {
        try {
            _dbusService.StopDiscovery();
        } catch (...) {}
        _scanStartedSuccessfully = false;
        return;
    }

    _scanStartedSuccessfully = true;
    _scanDesired = true;
}

void PassiveHciBasedBleAdvertisingService::StopScan()
{
    _scanDesired = false;

    if (_isRunning)
    {
        _isRunning = false;

        //Close socket before join() to avoid blocking
        CloseHciDevice();

        if (_listenThread.joinable())
            _listenThread.join();
    }
    else
    {
        CloseHciDevice();
    }

    if (_scanStartedSuccessfully)
    {
        // Use async to avoid stuck
        _dbusService.StopDiscoveryAsync(
            [](const sdbus::Error* error) {
                if (error) {
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
