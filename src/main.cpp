// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include <iostream>
#include <atomic>
#include <nlohmann/json.hpp>
#include <App.h>

#include "DevicesInfoFetcher.h"
#include "device/DeviceBattery.h"
#include "device/AapDevice.h"
#include "device/enums/DeviceAncModes.h"
#include "tests/TestsSgb.h"
#include "tests/TestsAapBle.h"
#include "Logger.h"
#include "Config.h"
#include "settings/SettingsService.h"
#include "settings/JsonToTomlConverter.h"

using namespace MagicPodsCore;

//Do not forget to change version when API changes
constexpr int API_VERSION = 0;
constexpr int WEBSOCKET_PORT = 2020;

nlohmann::json MakeGetDeviceResponse(DevicesInfoFetcher& devicesInfoFetcher) {
    auto rootObject = nlohmann::json::object();
    auto jsonArray = nlohmann::json::array();
    for (const auto& device : devicesInfoFetcher.GetDevices()) {
        auto deviceJson = device->GetAsJson();
        deviceJson.erase("capabilities"); // we do not need capabilities in GetDevices
        jsonArray.push_back(deviceJson);
    }
    rootObject["headphones"] = jsonArray;

    return rootObject;
}

nlohmann::json MakeGetActiveDeviceInfoResponse(DevicesInfoFetcher& devicesInfoFetcher) {
    auto activeDevice = devicesInfoFetcher.GetActiveDevice();
    if (activeDevice)
        return nlohmann::json{{"info", activeDevice->GetAsJson()}};

    return nlohmann::json{{"info", nlohmann::json::object()}};
}

nlohmann::json MakeGetDefaultBluetoothAdapterResponse(DevicesInfoFetcher& devicesInfoFetcher) {
    auto responseJson = nlohmann::json::object();
    auto defaultBluetoothJson = nlohmann::json::object();
    defaultBluetoothJson["enabled"] = devicesInfoFetcher.IsBluetoothAdapterPowered();
    responseJson["defaultbluetooth"] = defaultBluetoothJson;
    return responseJson;
}

nlohmann::json MakeGetSettingsAllResponse(SettingsService& settingsService) {
    auto wrappedSettings = settingsService.GetSettingsAllWrapped();
    return TomlToJson(wrappedSettings);
}

nlohmann::json MakeGetSettingsResponse(SettingsService& settingsService, const std::string& container) {
    auto containerSettings = settingsService.GetSettings(container);
    auto wrappedJson = nlohmann::json::object();
    wrappedJson["settings"] = nlohmann::json::object();
    wrappedJson["settings"][container] = TomlToJson(containerSettings);
    return wrappedJson;
}

nlohmann::json MakeGetSettingResponse(SettingsService& settingsService, const std::string& container, const std::string& setting) {
    auto settingValue = settingsService.GetSetting(container, setting);

    auto wrappedJson = nlohmann::json::object();
    wrappedJson["settings"] = nlohmann::json::object();
    wrappedJson["settings"][container] = nlohmann::json::object();
    wrappedJson["settings"][container][setting] = TomlNodeViewToJson(settingValue);

    return wrappedJson;
}

void InitHandshake(auto *ws) {
    Logger::Info("InitHandshake");
    auto wrappedJson = nlohmann::json::object();
    wrappedJson["init"]["api"] = API_VERSION;
    wrappedJson["init"]["version"] = CMAKE_PROJECT_VERSION;
    auto response = wrappedJson.dump();
    ws->send(response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
}

void HandleGetDevicesRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleGetDevicesRequest");

    auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleConnectDeviceRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleConnectDeviceRequest");

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
    Logger::Info("HandleDisconnectDeviceRequest");

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

void HandleSetCapabilitiesRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleSetAncRequest");
    devicesInfoFetcher.SetCapabilities(json);
}


void HandleGetDefaultBluetoothAdapterRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleGetDefaultBluetoothAdapterRequest");

    auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleEnableDefaultBluetoothAdapter(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleEnableDefaultBluetoothAdapter");

    devicesInfoFetcher.EnableBluetoothAdapterAsync([ws, &app, &devicesInfoFetcher, opCode](const sdbus::Error* error) {
        app.getLoop()->defer([ws, &app, &devicesInfoFetcher, opCode](){
            auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
            ws->send(response, opCode, response.length() < 16 * 1024);
        });
    });
}

void HandleDisableDefaultBluetoothAdapter(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleDisableDefaultBluetoothAdapter");

    devicesInfoFetcher.DisableBluetoothAdapterAsync([ws, &app, &devicesInfoFetcher, opCode](const sdbus::Error* error) {
        app.getLoop()->defer([ws, &app, &devicesInfoFetcher, opCode](){
            auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
            ws->send(response, opCode, response.length() < 16 * 1024);
        });
    });
}

void HandleGetActiveDeviceInfoRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleGetActiveDeviceInfoRequest");

    auto response = MakeGetActiveDeviceInfoResponse(devicesInfoFetcher).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleGetSettingsAllRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, SettingsService& settingsService) {
    Logger::Info("HandleGetSettingsAllRequest");

    auto response = MakeGetSettingsAllResponse(settingsService).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleGetSettingsRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, SettingsService& settingsService) {
    Logger::Info("HandleGetSettingsRequest");

    auto container = json.at("arguments").at("container").template get<std::string>();
    auto response = MakeGetSettingsResponse(settingsService, container).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleGetSettingRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, SettingsService& settingsService) {
    Logger::Info("HandleGetSettingRequest");

    auto container = json.at("arguments").at("container").template get<std::string>();
    auto setting = json.at("arguments").at("setting").template get<std::string>();
    auto response = MakeGetSettingResponse(settingsService, container, setting).dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleSaveSettingRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, SettingsService& settingsService) {
    Logger::Info("HandleSaveSettingRequest");

    auto container = json.at("arguments").at("container").template get<std::string>();
    auto setting = json.at("arguments").at("setting").template get<std::string>();
    auto value = json.at("arguments").at("value");

    if (value.is_boolean()) {
        settingsService.SaveSetting(container, setting, value.get<bool>());
    } else if (value.is_number_integer()) {
        settingsService.SaveSetting(container, setting, value.get<int64_t>());
    } else if (value.is_number_float()) {
        settingsService.SaveSetting(container, setting, value.get<double>());
    } else if (value.is_string()) {
        settingsService.SaveSetting(container, setting, value.get<std::string>());
    } else if (value.is_array()) {
        auto tomlArray = JsonArrayToToml(value);
        settingsService.SaveSetting(container, setting, tomlArray);
    } else if (value.is_object()) {
        auto tomlTable = JsonObjectToToml(value);
        settingsService.SaveSetting(container, setting, tomlTable);
    }
}

void HandleGetAllRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    Logger::Info("HandleGetAll");

    auto rootObject = nlohmann::json::object();
    auto getDevicesResponseJson = MakeGetDeviceResponse(devicesInfoFetcher);
    auto getDefaultBluetoothAdapterJson = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher);
    auto getDeckyInfoResponseJson = MakeGetActiveDeviceInfoResponse(devicesInfoFetcher);
    rootObject["headphones"] = getDevicesResponseJson["headphones"];
    rootObject["defaultbluetooth"] = getDefaultBluetoothAdapterJson["defaultbluetooth"];
    rootObject["info"] = getDeckyInfoResponseJson["info"];

    auto response = rootObject.dump();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleSetLogLevel(auto *ws,const nlohmann::json& json, uWS::OpCode opCode) {
    Logger::Info("HandleSetLogLevel");

    auto selected = json.at("arguments").at("selected").template get<int>();
    Logger::SetLoggingLevelForGlobalLogger(selected);
    //std::string emptyResponse{};
    //ws->send(emptyResponse, opCode, emptyResponse.length() < 16 * 1024);
}


void HandleIncorrectResponse(auto *ws, uWS::OpCode opCode) {
    std::string emptyResponse{};
    ws->send(emptyResponse, opCode, emptyResponse.length() < 16 * 1024);
}

void HandleRequest(auto *ws, std::string_view message, uWS::OpCode opCode, uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher, SettingsService& settingsService) {
    try {
        auto json = nlohmann::json::parse(message);

        std::string methodName = json.at("method").template get<std::string>();
            if (methodName == "GetDevices")
                HandleGetDevicesRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "ConnectDevice")
                HandleConnectDeviceRequest(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "DisconnectDevice")
                HandleDisconnectDeviceRequest(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "SetCapabilities")
                HandleSetCapabilitiesRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetDefaultBluetoothAdapter")
                HandleGetDefaultBluetoothAdapterRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "EnableDefaultBluetoothAdapter")
                HandleEnableDefaultBluetoothAdapter(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "DisableDefaultBluetoothAdapter")
                HandleDisableDefaultBluetoothAdapter(ws, json, opCode, app, devicesInfoFetcher);
            else if (methodName == "GetActiveDeviceInfo")
                HandleGetActiveDeviceInfoRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetAll")
                HandleGetAllRequest(ws, json, opCode, devicesInfoFetcher);
            else if (methodName == "GetSettingsAll")
                HandleGetSettingsAllRequest(ws, json, opCode, settingsService);
            else if (methodName == "GetSettings")
                HandleGetSettingsRequest(ws, json, opCode, settingsService);
            else if (methodName == "GetSetting")
                HandleGetSettingRequest(ws, json, opCode, settingsService);
            else if (methodName == "SetSetting")
                HandleSaveSettingRequest(ws, json, opCode, settingsService);
            else if (methodName == "SetLogLevel")
                HandleSetLogLevel(ws, json, opCode);
            else
                HandleIncorrectResponse(ws, opCode);
    }
    catch(const std::exception& exception) {
        HandleIncorrectResponse(ws, opCode);
    }
}

void SubscribeAndHandleBroadcastEvents(uWS::App& app, DevicesInfoFetcher& devicesInfoFetcher, SettingsService& settingsService) {
    auto onCapabilityChangedListener = [&app, &devicesInfoFetcher](std::shared_ptr<Device> device, const Capability& newValues) {
        if (auto activeDevice = devicesInfoFetcher.GetActiveDevice(); activeDevice && activeDevice->GetAddress() == device->GetAddress()) {
            Logger::Info("onCapabilityChanged Broadcast was triggered");
            app.getLoop()->defer([&app, &devicesInfoFetcher](){
                auto response = MakeGetActiveDeviceInfoResponse(devicesInfoFetcher).dump();
                app.publish("onCapabilityChanged", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
            });
        }
    };

    auto onConnectedChangedListener = [&app, &devicesInfoFetcher](std::shared_ptr<Device> device, bool newValue) {
        Logger::Info("OnConnectedChanged Broadcast was triggered");
        app.getLoop()->defer([&app, &devicesInfoFetcher]() {
            auto response = MakeGetDeviceResponse(devicesInfoFetcher).dump();
            app.publish("OnConnectedChanged", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };
    auto onActiveDeviceChanged = [&app, &devicesInfoFetcher](std::shared_ptr<Device> device) {
        Logger::Info("OnActiveDeviceChanged Broadcast was triggered");
        app.getLoop()->defer([&app, &devicesInfoFetcher]() {
            auto response = MakeGetActiveDeviceInfoResponse(devicesInfoFetcher).dump();
            app.publish("OnActiveDeviceChanged", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };
    auto onDefaultAdapterChangeEnabled = [&app, &devicesInfoFetcher](bool newValue) {
        Logger::Info("OnDefaultAdapterChangeEnabled Broadcast was triggered");
        app.getLoop()->defer([&app, &devicesInfoFetcher]() {
            auto response = MakeGetDefaultBluetoothAdapterResponse(devicesInfoFetcher).dump();
            app.publish("OnDefaultAdapterChangeEnabled", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };

    auto onAnimationTriggered = [&app](nlohmann::json newValue) {
        Logger::Info("onAnimationTriggered Broadcast was triggered");
        app.getLoop()->defer([&app, newValue]() {
            auto response = newValue.dump();
            app.publish("onAnimationTriggered", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };

    for (auto& device : devicesInfoFetcher.GetDevices()) {
        device->GetCapabilityChangedEvent().Subscribe([onCapabilityChangedListener, weakDevice = std::weak_ptr(device)](size_t listenerId, auto& newValues) {
            onCapabilityChangedListener(weakDevice.lock(), newValues);
        });
        device->GetConnectedPropertyChangedEvent().Subscribe([onConnectedChangedListener, weakDevice = std::weak_ptr(device)](size_t listenerId, auto newValue) {
            onConnectedChangedListener(weakDevice.lock(), newValue);
        });

        if (auto aap = std::dynamic_pointer_cast<AapDevice>(device)){
            aap->GetAnimationTriggered().Subscribe([onAnimationTriggered](size_t listenerId, const nlohmann::json &newValue){
                onAnimationTriggered(newValue);
            });
        }
    }
    devicesInfoFetcher.GetOnDeviceAddEvent().Subscribe([onCapabilityChangedListener, onConnectedChangedListener](size_t listenerId, auto device) {
        device->GetCapabilityChangedEvent().Subscribe([onCapabilityChangedListener, weakDevice = std::weak_ptr(device)](size_t listenerId, auto& newValues) {
            onCapabilityChangedListener(weakDevice.lock(), newValues);
        });
        device->GetConnectedPropertyChangedEvent().Subscribe([onConnectedChangedListener, weakDevice = std::weak_ptr(device)](size_t listenerId, auto newValue) {
            onConnectedChangedListener(weakDevice.lock(), newValue);
        });
    });
    devicesInfoFetcher.GetOnActiveDeviceChangedEvent().Subscribe([onActiveDeviceChanged](size_t listenerId, auto newDevice) {
        onActiveDeviceChanged(newDevice);
    });
    devicesInfoFetcher.GetOnDefaultAdapterChangeEnabledEvent().Subscribe([onDefaultAdapterChangeEnabled](size_t listenerId, bool newValue) {
        onDefaultAdapterChangeEnabled(newValue);
    });

    auto onSettingUpdate = [&app, &settingsService](const UpdatedSettingNotification& notification) {
        Logger::Info("OnSettingUpdate Broadcast was triggered");

        std::string container{notification.GetContainerName()};
        std::string setting{notification.GetSettingName()};

        app.getLoop()->defer([&app, &settingsService, container, setting]() {
            auto response = MakeGetSettingResponse(settingsService, container, setting).dump();
            app.publish("OnSettingUpdate", response, uWS::OpCode::TEXT, response.length() < 16 * 1024);
        });
    };

    settingsService.GetOnSettingUpdateEvent().Subscribe([onSettingUpdate](size_t listenerId, const UpdatedSettingNotification& notification) {
        onSettingUpdate(notification);
    });
}

bool TryToParseArguments(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        std::string argument{argv[i]};
        if (argument == "--version" || argument == "-version" || argument == "-v") {
            Logger::Info(CMAKE_PROJECT_NAME " " CMAKE_PROJECT_VERSION);
            return true;
        }
        else if (argument == "--help" || argument == "-help" || argument == "-h") {
            Logger::Info(
                "Help is under development. For more information and to contact us, please visit magicpods.app.\n"
                "Developed by Aleksandr Maslov<MagicPods@outlook.com> and Andrei Litvintsev<a.a.litvintsev@gmail.com>");
            return true;
        }
    }
    return false;
}

void StartListeningLogSettings(SettingsService &settingsService) {
    #ifdef DEBUG
    Logger::SetLoggingLevelForGlobalLogger(LogLevel::Debug);
    return;
    #endif

    const auto currentSettingsLogLevel = settingsService.GetSetting("magicpods", "logLevel");
    if (currentSettingsLogLevel.is_integer()) {
        Logger::SetLoggingLevelForGlobalLogger(currentSettingsLogLevel.as_integer()->get());
    }

    settingsService.GetOnSettingUpdateEvent().Subscribe([](size_t listenerId, const UpdatedSettingNotification& notification) {
        if (notification.GetContainerName() == "magicpods" && notification.GetSettingName() == "logLevel") {
            if (notification.GetValue().is_integer()) {
                Logger::SetLoggingLevelForGlobalLogger(notification.GetValue().as_integer()->get());
            }
        }
    });
}

int main(int argc, char** argv) {
    if (TryToParseArguments(argc, argv))
        return 0;

    std::shared_ptr<SettingsService> settingsService = std::make_shared<SettingsService>(SettingsService::GetConfigPath("config.toml"));
    StartListeningLogSettings(*settingsService);

    // fix stdout buffering issue, when python does not receive output
    setvbuf(stdout, NULL, _IONBF, 0);

    Logger::Info(CMAKE_PROJECT_NAME " " CMAKE_PROJECT_VERSION);

    std::atomic<bool> initialized{false};

    std::unique_ptr<DevicesInfoFetcher> devicesInfoFetcher;

    /* ws->getUserData returns one of these */
    struct PerSocketData {
        /* Fill with user data */
    };

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App app{};

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
        .open = [&initialized](auto* ws) {
            if (!initialized.load()) {
                Logger::Info("Connection attempted before initialization complete, closing");
                ws->close();
                return;
            }
            Logger::Info("On open websocket connected");
            ws->subscribe("onCapabilityChanged");
            ws->subscribe("OnConnectedChanged");
            ws->subscribe("OnActiveDeviceChanged");
            ws->subscribe("OnDefaultAdapterChangeEnabled");
            ws->subscribe("onAnimationTriggered");
            ws->subscribe("OnSettingUpdate");
            InitHandshake(ws);
        },
        .message = [&app, &devicesInfoFetcher, &settingsService](auto *ws, std::string_view message, uWS::OpCode opCode) {
            HandleRequest(ws, message, opCode, app, *devicesInfoFetcher, *settingsService);

            /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
             * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for
             * benchmarking of large message sending without compression */
            Logger::Info("Received: %s", std::string{message}.c_str());
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
            Logger::Info("On open websocket closed");
        }
    }).listen(WEBSOCKET_PORT, LIBUS_LISTEN_EXCLUSIVE_PORT, [&](auto *listen_socket) {
        if (listen_socket) {
            Logger::Info("Listening on port %d", WEBSOCKET_PORT);

            Logger::Info("Starting initialization...");

            #ifdef DEBUG
            TestsSgb sgb;
            TestsAapBle aapBle;
            #endif

            devicesInfoFetcher = std::make_unique<DevicesInfoFetcher>(settingsService);

            SubscribeAndHandleBroadcastEvents(app, *devicesInfoFetcher, *settingsService);

            Logger::Info("Initialization complete, ready to accept connections");
            initialized.store(true);
        } else {
            Logger::Error("Failed to listen on port %d", WEBSOCKET_PORT);
            exit(1);
        }
    }).run();
}
