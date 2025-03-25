#include "./dbus/DBusService.h"

int main() {
    using namespace std::chrono_literals;
    using namespace MagicPodsCore;
    DBusService service{};
    auto devices = service.GetBtDevices();
    auto devices2 = service.GetBtDevices();

    std::this_thread::sleep_for(1000000s);

    return 0;
}

// #include <iostream>
// #include "client/Client.h"
// #include "Logger.h"

// #include <iostream>
// #include <sdbus-c++/sdbus-c++.h>
// #include <string>
// #include <map>



// void print_services(const std::string &deviceAddress) {
//     try {
//         // Создаем подключение к системной шине D-Bus
//         auto connection = sdbus::createSystemBusConnection();

//         // Создаем прокси-объект для org.bluez
//         auto proxy = sdbus::createProxy(*connection, "org.bluez", "/");

//         // Переменная для хранения результатов вызова D-Bus метода
//         std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects;

//         // Вызываем метод GetManagedObjects и сохраняем результат
//         proxy->callMethod("GetManagedObjects")
//             .onInterface("org.freedesktop.DBus.ObjectManager")
//             .storeResultsTo(managedObjects);

//         // Перебираем полученные объекты
//         for (const auto& [objectPath, interfaces] : managedObjects) {
//             // Ищем устройство с нужным адресом
//             for (const auto& [interfaceName, properties] : interfaces) {
//                 if (interfaceName == "org.bluez.Device1") {
//                     auto it = properties.find("Address");
//                     if (it != properties.end() && it->second.get<std::string>() == deviceAddress) {
//                         std::cout << "Found device: " << deviceAddress << " at " << objectPath << std::endl;

//                         // Проверяем доступные UUID сервиса устройства
//                         for (const auto& [iface, props] : interfaces) {
//                             std::cout << "  Interface: " << iface << std::endl;
//                             auto uuid_it = props.find("UUIDs");
//                             if (uuid_it != props.end()) {
//                                 auto uuids = uuid_it->second.get<std::vector<std::string>>();
//                                 for (const auto& uuid : uuids) {
//                                     std::cout << "    UUID: " << uuid << std::endl;
//                                 }
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     } catch (const sdbus::Error& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
// }

// // НЕ ИСПОЛЬЗОВАТЬ
// void print_rfcomm_services(const std::string &deviceAddress) {
//     // Создаем подключение к системной шине D-Bus
//     auto connection = sdbus::createSystemBusConnection();

//     // Создаем прокси-объект для org.bluez
//     auto proxy = sdbus::createProxy(*connection, "org.bluez", "/");

//     // Переменная для хранения результатов вызова D-Bus метода
//     std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects;

//     // Вызываем метод GetManagedObjects и сохраняем результат
//     proxy->callMethod("GetManagedObjects")
//         .onInterface("org.freedesktop.DBus.ObjectManager")
//         .storeResultsTo(managedObjects);

//     // Перебираем полученные объекты
//     for (const auto& [objectPath, interfaces] : managedObjects) {
//         // Ищем устройство с нужным адресом
//         for (const auto& [interfaceName, properties] : interfaces) {
//             if (interfaceName == "org.bluez.Device1") {
//                 auto it = properties.find("Address");
//                 if (it != properties.end() && it->second.get<std::string>() == deviceAddress) {
//                     std::cout << "Found device: " << deviceAddress << " at " << objectPath << std::endl;
                    
//                     // Проверяем доступные сервисы устройства
//                     bool rfcommFound = false;
//                     for (const auto& [iface, props] : interfaces) {
//                         std::cout << "  Interface: " << iface << std::endl;
//                         if (iface.find("Rfcomm") != std::string::npos) {
//                             std::cout << "  RFCOMM service detected!" << std::endl;
//                             rfcommFound = true;
//                         }
//                     }
//                     if (!rfcommFound) {
//                         std::cout << "  No RFCOMM service found on this device." << std::endl;
//                     }
//                 }
//             }
//         }
//     }
// }
// int main() {
//     using namespace MagicPodsCore;

//     std::cout << "Hello" << std::endl;
//     print_services("0A:E1:65:04:FF:B3");
//     return 0;
// }

// //GetBtDevices(); -> struct[]

// //struct -> device -> 
// //mac
// //Modalias as ProductID(p) (unsignet short) VendorId(v) (unsignet short)
// // UUIDS list
// //"00000000-0000-0000-0099-aabbccddeeff"
// //"00001108-0000-1000-8000-00805f9b34fb"
// //"0000110b-0000-1000-8000-00805f9b34fb"
// //"0000110c-0000-1000-8000-00805f9b34fb"
// //"0000110d-0000-1000-8000-00805f9b34fb"
// //"0000110e-0000-1000-8000-00805f9b34fb"
// //"0000111e-0000-1000-8000-00805f9b34fb"
// //"00001200-0000-1000-8000-00805f9b34fb"
// //"74ec2172-0bad-4d01-8f77-997b2be0722a"
// //Class
// //Name
// //Connection status



// //]# info 0A:E1:65:04:FF:B3
// //Device 0A:E1:65:04:FF:B3 (public)
// //        Name: cc ultra
// //        Alias: cc ultra
// //        Class: 0x00240404 (2360324) audio | render
// //        Icon: audio-headset
// //        Paired: yes
// //        Bonded: yes
// //        Trusted: yes
// //        Blocked: no
// //        Connected: no
// //        LegacyPairing: no
// //        +UUID: Vendor specific           (00000000-0000-0000-0099-aabbccddeeff)
// //        +UUID: Headset                   (00001108-0000-1000-8000-00805f9b34fb)
// //        +UUID: Audio Sink                (0000110b-0000-1000-8000-00805f9b34fb)
// //        +UUID: A/V Remote Control Target (0000110c-0000-1000-8000-00805f9b34fb)
// //        +UUID: Advanced Audio Distribu.. (0000110d-0000-1000-8000-00805f9b34fb)
// //        +UUID: A/V Remote Control        (0000110e-0000-1000-8000-00805f9b34fb)
// //        +UUID: Handsfree                 (0000111e-0000-1000-8000-00805f9b34fb)
// //        +UUID: PnP Information           (00001200-0000-1000-8000-00805f9b34fb)
// //        +UUID: Vendor specific           (74ec2172-0bad-4d01-8f77-997b2be0722a)
// //        +Modalias: bluetooth:v004Cp2014d4C1B
// //        ManufacturerData Key: 0x004c (76)
// //        ManufacturerData Value:
// //  10 06 6b 1e fd 63 ad 9e                          ..k..c..        
// //        RSSI: 0xffffffdc (-36)
// //        TxPower: 0x0008 (8)