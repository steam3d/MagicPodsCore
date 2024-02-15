#include <iostream>
#include <App.h>

#include "DevicesInfoFetcher.h"
#include "aap/Aap.h"
#include "aap/AapClient.h"
#include "DeviceBattery.h"

using namespace MagicPodsCore;

nlohmann::json MakeGetDeviceResponse(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    auto rootObject = nlohmann::json::object();
    auto jsonArray = nlohmann::json::array();
    for (const auto& device : devicesInfoFetcher.GetDevices()) {
        auto jsonObject = nlohmann::json::object();
        jsonObject["name"] = device->GetName();
        jsonObject["address"] = device->GetAddress();
        jsonObject["connected"] = device->GetConnected();
        jsonArray.push_back(jsonObject);
    }
    rootObject["headphones"] = jsonArray;

    return rootObject;
}

nlohmann::json MakeGetDeckyInfoResponse(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    auto activeDevice = devicesInfoFetcher.GetActiveDevice();

    auto rootObject = nlohmann::json::object();
    auto jsonObject = nlohmann::json::object();


    if (activeDevice) {
        jsonObject["name"] = activeDevice->GetName();
        jsonObject["address"] = activeDevice->GetAddress();
        jsonObject["connected"] = activeDevice->GetConnected();

        auto jsonBatteryObject = nlohmann::json::object();
        for (const auto& [batteryType, batteryData] : activeDevice->GetBatteryStatus()) {
            auto jsonBatteryData = nlohmann::json::object();
            switch (batteryType) {
                case DeviceBatteryType::Single:
                    jsonBatteryData["battery"] = batteryData.Battery;
                    jsonBatteryData["charging"] = batteryData.isCharging;
                    jsonBatteryData["status"] = batteryData.Status;
                    jsonBatteryObject["single"] = jsonBatteryData;                
                    break;

                case DeviceBatteryType::Right:
                    jsonBatteryData["battery"] = batteryData.Battery;
                    jsonBatteryData["charging"] = batteryData.isCharging;
                    jsonBatteryData["status"] = batteryData.Status;
                    jsonBatteryObject["right"] = jsonBatteryData;  
                    break;

                case DeviceBatteryType::Left:
                    jsonBatteryData["battery"] = batteryData.Battery;
                    jsonBatteryData["charging"] = batteryData.isCharging;
                    jsonBatteryData["status"] = batteryData.Status;
                    jsonBatteryObject["left"] = jsonBatteryData;  
                    break;

                case DeviceBatteryType::Case:
                    jsonBatteryData["battery"] = batteryData.Battery;
                    jsonBatteryData["charging"] = batteryData.isCharging;
                    jsonBatteryData["status"] = batteryData.Status;
                    jsonBatteryObject["case"] = jsonBatteryData;  
                    break;
            }
        }
        jsonObject["battery"] = jsonBatteryObject;
        
        // TODO: REMOVE THIS TWO LINES
        auto jsonBatteryObjectCap = nlohmann::json::object();
        jsonObject["capabilities"] = jsonBatteryObjectCap;
    }

    rootObject["info"] = jsonObject;

    return rootObject;
}

nlohmann::json MakeGetDefaultBluetoothAdapterResponse(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    auto responseJson = nlohmann::json::object();
    auto defaultBluetoothJson = nlohmann::json::object();
    defaultBluetoothJson["enabled"] = devicesInfoFetcher.IsBluetoothAdapterPowered();
    responseJson["defaultbluetooth"] = defaultBluetoothJson;
    return responseJson;
}

void HandleGetDevicesRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleGetDevicesRequest" << std::endl; // TODO: delete

    auto response = MakeGetDeviceResponse(ws, json, opCode, devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleConnectDeviceRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleConnectDeviceRequest" << std::endl; // TODO: delete

    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    devicesInfoFetcher.Connect(deviceAddress);

    auto response = MakeGetDeviceResponse(ws, json, opCode, devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleDisconnectDeviceRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleDisconnectDeviceRequest" << std::endl; // TODO: delete

    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    devicesInfoFetcher.Disconnect(deviceAddress);

    auto response = MakeGetDeviceResponse(ws, json, opCode, devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleSetAncRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleSetAncRequest" << std::endl; // TODO: delete
    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    auto value = json.at("arguments").at("value").template get<int>();

    if (value == 1 || value == 2 || value == 3) // TODO: исправить костыль
        devicesInfoFetcher.SetAnc(deviceAddress, static_cast<AncMode>(value));
}

void HandleGetDefaultBluetoothAdapterRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleGetDefaultBluetoothAdapterRequest" << std::endl; // TODO: delete
    

    auto response = MakeGetDefaultBluetoothAdapterResponse(ws, json, opCode, devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleEnableDefaultBluetoothAdapter(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleEnableDefaultBluetoothAdapter" << std::endl; // TODO: delete

    devicesInfoFetcher.EnableBluetoothAdapter();

    std::cout << "HandledEnableDefaultBluetoothAdapter" << std::endl; // TODO: delete

    auto responseJson = nlohmann::json::object(); // TODO: убрать дублирование
    auto defaultBluetoothJson = nlohmann::json::object();
    defaultBluetoothJson["enabled"] = devicesInfoFetcher.IsBluetoothAdapterPowered();
    responseJson["defaultbluetooth"] = defaultBluetoothJson;

    auto response = responseJson.dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleDisableDefaultBluetoothAdapter(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleDisableDefaultBluetoothAdapter" << std::endl; // TODO: delete

    devicesInfoFetcher.DisableBluetoothAdapter();

    auto responseJson = nlohmann::json::object(); // TODO: убрать дублирование
    auto defaultBluetoothJson = nlohmann::json::object();
    defaultBluetoothJson["enabled"] = devicesInfoFetcher.IsBluetoothAdapterPowered();
    responseJson["defaultbluetooth"] = defaultBluetoothJson;

    auto response = responseJson.dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleGetDeckyInfoRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleGetDeckyInfoRequest" << std::endl; // TODO: delete

    auto response = MakeGetDeckyInfoResponse(ws, json, opCode, devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleGetAllRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleGetAll" << std::endl; // TODO: delete

    auto rootObject = nlohmann::json::object();
    auto getDevicesResponseJson = MakeGetDeviceResponse(ws, json, opCode, devicesInfoFetcher);
    auto getDefaultBluetoothAdapterJson = MakeGetDefaultBluetoothAdapterResponse(ws, json, opCode, devicesInfoFetcher);
    auto getDeckyInfoResponseJson = MakeGetDeckyInfoResponse(ws, json, opCode, devicesInfoFetcher);
    rootObject["headphones"] = getDevicesResponseJson["headphones"];
    rootObject["defaultbluetooth"] = getDefaultBluetoothAdapterJson["defaultbluetooth"];
    rootObject["info"] = getDeckyInfoResponseJson["info"];

    auto response = rootObject.dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleRequest(auto *ws, std::string_view message, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    try {
        auto json = nlohmann::json::parse(message);

        std::string methodName = json.at("method").template get<std::string>();
            if (methodName == "GetDevices")
                HandleGetDevicesRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "ConnectDevice")
                HandleConnectDeviceRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "DisconnectDevice")
                HandleDisconnectDeviceRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "SetAnc")
                HandleSetAncRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetDefaultBluetoothAdapter")
                HandleGetDefaultBluetoothAdapterRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "EnableDefaultBluetoothAdapter")
                HandleEnableDefaultBluetoothAdapter(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "DisableDefaultBluetoothAdapter")
                HandleDisableDefaultBluetoothAdapter(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetDeckyInfo")
                HandleGetDeckyInfoRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetAll")
                HandleGetAllRequest(ws, json, opCode, devicesInfoFetcher);
    }
    catch(const std::exception& exception) {
        // ignoring incorrect json
    }
}

int main() {
    DevicesInfoFetcher devicesInfoFetcher{};

    /* ws->getUserData returns one of these */
    struct PerSocketData {
        /* Fill with user data */
    };

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App().ws<PerSocketData>("/*", {
        /* Settings */
        .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR_4KB | uWS::DEDICATED_DECOMPRESSOR),
        .maxPayloadLength = 100 * 1024 * 1024,
        .idleTimeout = 16,
        .maxBackpressure = 100 * 1024 * 1024,
        .closeOnBackpressureLimit = false,
        .resetIdleTimeoutOnSend = false,
        .sendPingsAutomatically = true,
        /* Handlers */
        .upgrade = nullptr,
        .open = [](auto */*ws*/) {
            /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */

        },
        .message = [&devicesInfoFetcher](auto *ws, std::string_view message, uWS::OpCode opCode) {
            HandleRequest(ws, message, opCode, devicesInfoFetcher);

            /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
             * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for 
             * benchmarking of large message sending without compression */
            std::cout << "Received:" << message << std::endl; // TODO: delete
        },
        .dropped = [](auto */*ws*/, std::string_view /*message*/, uWS::OpCode /*opCode*/) {
            /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
        },
        .drain = [](auto */*ws*/) {
            /* Check ws->getBufferedAmount() here */
        },
        .ping = [](auto */*ws*/, std::string_view) {
            /* Not implemented yet */
        },
        .pong = [](auto */*ws*/, std::string_view) {
            /* Not implemented yet */
        },
        .close = [](auto */*ws*/, int /*code*/, std::string_view /*message*/) {
            /* You may access ws->getUserData() here */
        }
    }).listen(9001, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port " << 9001 << std::endl;
        }
    }).run();
}