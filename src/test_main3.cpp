#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <iomanip>
#include <sstream>
#include <unistd.h>

#include "dbus/DBusService.h"
#include "ble_ads/DBusBasedBleAdvertisingService.h"
#include "ble_ads/PassiveHciBasedBleAdvertisingService.h"
#include "ble_ads/BleAdverisingData.h"

// Global flag to handle termination
volatile sig_atomic_t g_running = 1;

void signal_handler(int signal) {
    g_running = 0;
}

// Helper to format manufacturer data for printing
std::string format_mfg_data(const std::map<uint16_t, std::vector<uint8_t>>& data) {
    if (data.empty()) {
        return "  (none)";
    }
    std::stringstream ss;
    for (const auto& [company_id, bytes] : data) {
        ss << "\n    - Company ID: 0x" << std::hex << company_id << std::dec;
        ss << ", Data: ";
        for (const auto& byte : bytes) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
    }
    return ss.str();
}


int main(int argc, char* argv[]) {
    // Set up signal handler for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Automatically determine which service to use based on privileges
    bool hasRootPrivileges = (geteuid() == 0);

    std::cout << "========================================" << std::endl;
    std::cout << "BLE Advertising Service Test" << std::endl;
    std::cout << "Mode: ";
    if (hasRootPrivileges) {
        std::cout << "Passive HCI (Safe, Low Latency <10ms)" << std::endl;
    } else {
        std::cout << "DBus-based (No Root Required, ~50-100ms)" << std::endl;
    }
    std::cout << "========================================" << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;
    std::cout << std::endl;

    if (hasRootPrivileges) {
        std::cout << "Running with root privileges." << std::endl;
        std::cout << "Using Passive HCI mode for low latency (<10ms)." << std::endl;
        std::cout << "SAFE: Works alongside bluetoothd and other BT apps." << std::endl;
        std::cout << std::endl;
    } else {
        std::cout << "Running without root privileges." << std::endl;
        std::cout << "Using DBus mode (latency ~50-100ms)." << std::endl;
        std::cout << "TIP: Run with 'sudo' for better performance." << std::endl;
        std::cout << std::endl;
    }

    try {
        // 1. Instantiate DBusService
        MagicPodsCore::DBusService dbusService;

        if (hasRootPrivileges) {
            // Use Passive HCI-based service (SAFE, low latency)
            MagicPodsCore::PassiveHciBasedBleAdvertisingService bleService(dbusService, 0); // 0 = hci0

            // 2. Subscribe to the event
            bleService.GetOnAdReceivedEvent().Subscribe(
                [](size_t, const MagicPodsCore::BleAdertisingData& adData) {
                    std::cout << "[Passive HCI] Received Ad:"
                              << "\n  - Address: " << adData.GetAddress()
                              << "\n  - RSSI: " << static_cast<int>(adData.GetRssi())
                              << "\n  - Manufacturer Data:" << format_mfg_data(adData.GetManufacturerData())
                              << std::endl;
                }
            );

            // 3. Start scanning and listening
            bleService.StartScan(false); // isPassive = false (active scan via DBus)
            bleService.StartListening();

            std::cout << "Passive HCI scanning started. Waiting for advertisements..." << std::endl;

            // 4. Keep the main thread alive
            while (g_running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            // 5. Clean up
            std::cout << "\nStopping scan..." << std::endl;
            bleService.StopScan();
            std::cout << "Exiting." << std::endl;

        } else {
            // Use DBus-based service (no root required)
            MagicPodsCore::DBusBasedBleAdvertisingService bleService(dbusService);

            // 2. Subscribe to the event
            bleService.GetOnAdReceivedEvent().Subscribe(
                [](size_t, const MagicPodsCore::BleAdertisingData& adData) {
                    std::cout << "[DBus] Received Ad:"
                              << "\n  - Address: " << adData.GetAddress()
                              << "\n  - RSSI: " << static_cast<int>(adData.GetRssi())
                              << "\n  - Manufacturer Data:" << format_mfg_data(adData.GetManufacturerData())
                              << std::endl;
                }
            );

            // 3. Start listening and scanning
            bleService.StartListening();
            bleService.StartScan(false); // isPassive = false (active scan)

            std::cout << "DBus scanning started. Waiting for advertisements..." << std::endl;

            // 4. Keep the main thread alive
            while (g_running) {
                // The sdbus-c++ connection's event loop needs to process events.
                // A simple sleep is enough to keep the main thread from exiting
                // while background threads (if any) or the D-Bus proxy handle events.
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            // 5. Clean up
            std::cout << "\nStopping scan..." << std::endl;
            bleService.StopScan();
            std::cout << "Exiting." << std::endl;
        }

    } catch (const sdbus::Error& e) {
        std::cerr << "sdbus error: " << e.getName() << " - " << e.getMessage() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
