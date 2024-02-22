#include <iostream>
#include <App.h>
#include <json.hpp>

#include "DevicesInfoFetcher.h"
#include "aap/Aap.h"
#include "aap/AapClient.h"
#include "DeviceBattery.h"
#include "DeviceAnc.h"
#include "Logger.h"

using namespace MagicPodsCore;

nlohmann::json MakeGetDeviceResponse(DevicesInfoFetcher& devicesInfoFetcher) {
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

nlohmann::json MakeGetDeckyInfoResponse(DevicesInfoFetcher& devicesInfoFetcher) {
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
            jsonBatteryData["battery"] = batteryData.Battery;
            jsonBatteryData["charging"] = batteryData.isCharging;
            jsonBatteryData["status"] = batteryData.Status;
            
            switch (batteryType) {
                case DeviceBatteryType::Single:
                    jsonBatteryObject["single"] = jsonBatteryData;                
                    break;

                case DeviceBatteryType::Right:
                    jsonBatteryObject["right"] = jsonBatteryData;  
                    break;

                case DeviceBatteryType::Left:
                    jsonBatteryObject["left"] = jsonBatteryData;  
                    break;

                case DeviceBatteryType::Case:
                    jsonBatteryObject["case"] = jsonBatteryData;  
                    break;
            }
        }
        jsonObject["battery"] = jsonBatteryObject;
        
        // TODO: REFACTOR THIS
        auto jsonBatteryObjectCap = nlohmann::json::object();
        auto ancMode = activeDevice->GetAncMode();
        LOG_RELEASE("Get anc mode: %u", static_cast<unsigned char>(ancMode));
        if (ancMode != DeviceAncMode::NotAvailable){
            jsonBatteryObjectCap["anc"] = ancMode;
        }
        jsonObject["capabilities"] = jsonBatteryObjectCap;
    }

    rootObject["info"] = jsonObject;

    return rootObject;
}

nlohmann::json MakeGetDefaultBluetoothAdapterResponse(DevicesInfoFetcher& devicesInfoFetcher) {
    auto responseJson = nlohmann::json::object();
    auto defaultBluetoothJson = nlohmann::json::object();
    defaultBluetoothJson["enabled"] = devicesInfoFetcher.IsBluetoothAdapterPowered();
    responseJson["defaultbluetooth"] = defaultBluetoothJson;
    return responseJson;
}

void HandleGetDevicesRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleGetDevicesRequest");

    auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleConnectDeviceRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleConnectDeviceRequest");

    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    auto device = devicesInfoFetcher.GetDevice(deviceAddress);

    if (device == nullptr || device->GetConnected()) {
        auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
        ws->send(response, opCode, response.length() < 16 * 1024);
        return;
    }

    device->ConnectAsync([device, ws, &json, opCode, &app, &devicesInfoFetcher](const sdbus::Error* error) {
        app.getLoop()->defer([device, ws, &json, opCode, &app, &devicesInfoFetcher]() {
            auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
            ws->send(response, opCode, response.length() < 16 * 1024);
        });
    });
}

void HandleDisconnectDeviceRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleDisconnectDeviceRequest");

    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    auto device = devicesInfoFetcher.GetDevice(deviceAddress);

    if (device == nullptr || !device->GetConnected()) {
        auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
        ws->send(response, opCode, response.length() < 16 * 1024);
        return;
    }

    device->DisconnectAsync([device, ws, &json, opCode, &app, &devicesInfoFetcher](const sdbus::Error* error) {
        app.getLoop()->defer([device, ws, &json, opCode, &app, &devicesInfoFetcher]() {
            auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
            ws->send(response, opCode, response.length() < 16 * 1024);
        });
    });
}

void HandleSetAncRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleSetAncRequest");

    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    auto value = json.at("arguments").at("value").template get<int>();

    if (value == 1 || value == 2 || value == 3) // TODO: исправить костыль
        devicesInfoFetcher.SetAnc(deviceAddress, static_cast<DeviceAncMode>(value));
}

void HandleGetDefaultBluetoothAdapterRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleGetDefaultBluetoothAdapterRequest");
    
    auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleEnableDefaultBluetoothAdapter(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleEnableDefaultBluetoothAdapter");

    devicesInfoFetcher.EnableBluetoothAdapterAsync([ws, &app, &devicesInfoFetcher, opCode](const sdbus::Error* error) {
        app.getLoop()->defer([ws, &app, &devicesInfoFetcher, opCode](){
            auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
            ws->send(response, opCode, response.length() < 16 * 1024);
        });
    });
}

void HandleDisableDefaultBluetoothAdapter(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleDisableDefaultBluetoothAdapter");

    devicesInfoFetcher.DisableBluetoothAdapterAsync([ws, &app, &devicesInfoFetcher, opCode](const sdbus::Error* error) {
        app.getLoop()->defer([ws, &app, &devicesInfoFetcher, opCode](){
            auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
            ws->send(response, opCode, response.length() < 16 * 1024);
        });
    });
}

void HandleGetDeckyInfoRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleGetDeckyInfoRequest");

    auto response = MakeGetDeckyInfoResponse(devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleGetAllRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    LOG_RELEASE("HandleGetAll");

    auto rootObject = nlohmann::json::object();
    auto getDevicesResponseJson = MakeGetDeviceResponse(devicesInfoFetcher);
    auto getDefaultBluetoothAdapterJson = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher);
    auto getDeckyInfoResponseJson = MakeGetDeckyInfoResponse(devicesInfoFetcher);
    rootObject["headphones"] = getDevicesResponseJson["headphones"];
    rootObject["defaultbluetooth"] = getDefaultBluetoothAdapterJson["defaultbluetooth"];
    rootObject["info"] = getDeckyInfoResponseJson["info"];

    auto response = rootObject.dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleIncorrectResponse(auto *ws, uWS::OpCode opCode) {
    std::string emptyResponse{};
    ws->send(emptyResponse, opCode, emptyResponse.length() < 16 * 1024);
}

void HandleRequest(auto *ws, std::string_view message, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    try {
        auto json = nlohmann::json::parse(message);

        std::string methodName = json.at("method").template get<std::string>();
            if (methodName == "GetDevices")
                HandleGetDevicesRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "ConnectDevice")
                HandleConnectDeviceRequest(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "DisconnectDevice")
                HandleDisconnectDeviceRequest(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "SetAnc")
                HandleSetAncRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetDefaultBluetoothAdapter")
                HandleGetDefaultBluetoothAdapterRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "EnableDefaultBluetoothAdapter")
                HandleEnableDefaultBluetoothAdapter(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "DisableDefaultBluetoothAdapter")
                HandleDisableDefaultBluetoothAdapter(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "GetDeckyInfo")
                HandleGetDeckyInfoRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetAll")
                HandleGetAllRequest(ws, json, opCode, devicesInfoFetcher);
            else
                HandleIncorrectResponse(ws, opCode);
    }
    catch(const std::exception& exception) {
        HandleIncorrectResponse(ws, opCode);
    }
}

void SubscribeAndHandleBroadcastEvents(uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    auto onBatteryChangedListener = [&app, &devicesInfoFetcher](std::shared_ptr<Device> device, std::map<DeviceBatteryType, DeviceBatteryData> newValues) {
        if (auto activeDevice = devicesInfoFetcher.GetActiveDevice(); activeDevice && activeDevice->GetAddress() == device->GetAddress()) {
            LOG_RELEASE("OnBatteryChanged Broadcast was triggered");
            app.getLoop()->defer([&app, &devicesInfoFetcher](){
                auto response = MakeGetDeckyInfoResponse(devicesInfoFetcher).dump();
                app.publish("OnBatteryChanged", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
            });
        }
    };
    auto onAncChangedListener = [&app, &devicesInfoFetcher](std::shared_ptr<Device> device, DeviceAncMode newValue) {
        if (auto activeDevice = devicesInfoFetcher.GetActiveDevice(); activeDevice && activeDevice->GetAddress() == device->GetAddress()) {
            LOG_RELEASE("OnAncChanged Broadcast was triggered");
            app.getLoop()->defer([&app, &devicesInfoFetcher](){
                auto response = MakeGetDeckyInfoResponse(devicesInfoFetcher).dump();
                app.publish("OnAncChanged", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
            });
        }
    };
    auto onConnectedChangedListener = [&app, &devicesInfoFetcher](std::shared_ptr<Device> device, bool newValue) {
        LOG_RELEASE("OnConnectedChanged Broadcast was triggered");
        app.getLoop()->defer([&app, &devicesInfoFetcher]() {
            auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
            app.publish("OnConnectedChanged", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };
    auto onActiveDeviceChanged = [&app, &devicesInfoFetcher](std::shared_ptr<Device> device) {
        LOG_RELEASE("OnActiveDeviceChanged Broadcast was triggered");
        app.getLoop()->defer([&app, &devicesInfoFetcher]() {
            auto response = MakeGetDeckyInfoResponse(devicesInfoFetcher).dump();
            app.publish("OnActiveDeviceChanged", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };
    auto onDefaultAdapterChangeEnabled = [&app, &devicesInfoFetcher](bool newValue) {
        LOG_RELEASE("OnDefaultAdapterChangeEnabled Broadcast was triggered");
        app.getLoop()->defer([&app, &devicesInfoFetcher]() {
            auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
            app.publish("OnDefaultAdapterChangeEnabled", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };

    for (auto& device : devicesInfoFetcher.GetDevices()) {
        device->GetBattery().GetBatteryChangedEvent().Subscribe([onBatteryChangedListener, device](size_t listenerId, auto newValues) {
            onBatteryChangedListener(device, newValues);
        });
        device->GetAnc().GetAncChangedEvent().Subscribe([onAncChangedListener, device](size_t listenerId, auto newValue) {
            onAncChangedListener(device, newValue);
        });
        device->GetConnectedPropertyChangedEvent().Subscribe([onConnectedChangedListener, device](size_t listenerId, auto newValue) {
            onConnectedChangedListener(device, newValue);
        });
    }
    devicesInfoFetcher.GetOnDevicesAddEvent().Subscribe([onBatteryChangedListener, onAncChangedListener, onConnectedChangedListener](size_t listenerId, auto newDevices) {
        for (auto& device : newDevices) {
            device->GetBattery().GetBatteryChangedEvent().Subscribe([onBatteryChangedListener, device](size_t listenerId, auto newValues) {
                onBatteryChangedListener(device, newValues);
            });
            device->GetAnc().GetAncChangedEvent().Subscribe([onAncChangedListener, device](size_t listenerId, auto newValue) {
                onAncChangedListener(device, newValue);
            });
            device->GetConnectedPropertyChangedEvent().Subscribe([onConnectedChangedListener, device](size_t listenerId, auto newValue) {
                onConnectedChangedListener(device, newValue);
            });
        }
    });
    devicesInfoFetcher.GetOnActiveDeviceChangedEvent().Subscribe([onActiveDeviceChanged](size_t listenerId, auto newDevice) {
        onActiveDeviceChanged(newDevice);
    });
    devicesInfoFetcher.GetOnDefaultAdapterChangeEnabledEvent().Subscribe([onDefaultAdapterChangeEnabled](size_t listenerId, bool newValue) {
        onDefaultAdapterChangeEnabled(newValue);
    });
}

bool TryToParseArguments(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        std::string argument{argv[i]};
        if (argument == "--version" || argument == "-version" || argument == "-v") {
            LOG_RELEASE("MagicPodsCore 1.0.0");
            return true;
        }
        else if (argument == "--help" || argument == "-help" || argument == "-h" || argc > 2) {
            LOG_RELEASE(
                "Help is under development. For more information and to contact us, please visit magicpods.app.\n"
                "Developed by Aleksandr Maslov<MagicPods@outlook.com> and Andrei Litvintsev<a.a.litvintsev@gmail.com>");
            return true;
        }
    }
    return false;
}

int main(int argc, char** argv) {
    if (TryToParseArguments(argc, argv))
        return 0;

    DevicesInfoFetcher devicesInfoFetcher{};

    /* ws->getUserData returns one of these */
    struct PerSocketData {
        /* Fill with user data */
    };

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App app{};

    SubscribeAndHandleBroadcastEvents(app, devicesInfoFetcher);

    app.ws<PerSocketData>("/*", {
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
        .open = [](auto* ws) {
            /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */
            LOG_RELEASE("On open websocket connected");
            ws->subscribe("OnBatteryChanged");
            ws->subscribe("OnAncChanged");
            ws->subscribe("OnConnectedChanged");
            ws->subscribe("OnActiveDeviceChanged");
            ws->subscribe("OnDefaultAdapterChangeEnabled");
        },
        .message = [&app, &devicesInfoFetcher](auto *ws, std::string_view message, uWS::OpCode opCode) {
            HandleRequest(ws, message, opCode, app, devicesInfoFetcher);

            /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
             * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for 
             * benchmarking of large message sending without compression */
            LOG_RELEASE("Received: %s", std::string{message}.c_str());
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
            LOG_RELEASE("On open websocket closed");
        }
    }).listen(9001, [](auto *listen_socket) {
        if (listen_socket) {
            LOG_RELEASE("Listening on port %d", 9001);
        }
    }).run();
}