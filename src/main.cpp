#include <iostream>
#include <App.h>

#include "DevicesInfoFetcher.h"
#include "aap/Aap.h"
#include "aap/AapClient.h"

using namespace MagicPodsCore;

// void checkAap()
// {
//     AapInit init;
//     init.PrintAsHex();

//     AapEnableNotifications EnableNotifications(NotificationsMode::Unknown1);
//     EnableNotifications.PrintAsHex();

//     AapSetAnc setAnc(AncMode::Off);
//     setAnc.PrintAsHex();

//     //04000400040003020164020104016301010801110201
//     unsigned char* barr22 = new unsigned char[22] { 4, 0, 4, 0, 4, 0, 3, 2, 1, 100, 2, 1, 4, 1, 99, 1, 1, 8, 1, 17, 2, 1};
//     //0400040004000101015d0101
//     unsigned char* barr12 = new unsigned char[22] { 4, 0, 4, 0, 4, 0, 1, 1, 1, 93, 1, 1};
//     AapBatteryWatcher batteryWatcher;
//     batteryWatcher.ProcessBytes(barr22, 22);
//     batteryWatcher.ProcessBytes(barr12, 22);

//     AapAncWatcher aapAncWatcher;
//     aapAncWatcher.ProcessBytes(setAnc.Request(), setAnc.GetLength());

//      printf("%04x \n", 0x4c00);
//      printf("%04x \n", 0x4c05);
//      printf("%d \n", 0x4c00);
//      printf("%d \n", static_cast<unsigned short>(AppleProductIds::airpods2));
//      printf("%04x \n", static_cast<unsigned short>(BtVendorIds::Apple));
// }

void l2capClient() {
    // AapClient client{};
    // client.Process();
}

void HandleGetDevicesRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleGetDevicesRequest" << std::endl; // TODO: delete

    auto response = devicesInfoFetcher.AsJson();
    ws->send(response, opCode, response.length() < 16 * 1024);
}

void HandleConnectDeviceRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleConnectDeviceRequest" << std::endl; // TODO: delete

    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    devicesInfoFetcher.Connect(deviceAddress);
}

void HandleDisconnectDeviceRequest(auto *ws, const nlohmann::json& json, uWS::OpCode opCode, DevicesInfoFetcher& devicesInfoFetcher) {
    std::cout << "HandleDisconnectDeviceRequest" << std::endl; // TODO: delete
    auto deviceAddress = json.at("arguments").at("address").template get<std::string>();
    devicesInfoFetcher.Disconnect(deviceAddress);
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
    }
    catch(const std::exception& exception) {
        // ignoring incorrect json
    }
}

int main() {
    l2capClient();

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