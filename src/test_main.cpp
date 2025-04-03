#include "client/Client.h"
#include "StringUtils.h"
#include "Logger.h"

int main() {
    using namespace MagicPodsCore;

    const auto client = Client::CreateRFCOMM("24:29:34:BD:CD:0F", "df21fe2c-2515-4fdb-8886-f12c4d67927c");
    client->GetOnReceivedDataEvent().Subscribe([](size_t listenerId, const auto& receivedBytes) {
        // do nothing
    });
    // client->Start();
    // or
    client->Start([](Client& client) {
        client.SendData(std::vector<unsigned char> { 0x08, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client.SendData(std::vector<unsigned char> { 0x08, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client.SendData(std::vector<unsigned char> { 0x08, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
    });

    client->SendData(std::vector<unsigned char> { 0x01, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
    client->SendData(std::vector<unsigned char> { 0x02, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
    client->SendData(std::vector<unsigned char> { 0x03, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
    client->SendData(std::vector<unsigned char> { 0x08, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
    std::this_thread::sleep_for(std::chrono::seconds(5));
    client->SendData(std::vector<unsigned char> { 0x08, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });
    std::this_thread::sleep_for(std::chrono::seconds(10));
    client->SendData(std::vector<unsigned char> { 0x08, 0x13, 0x00, 0x04, 0x01, 0xa8, 0x00, 0x20 });

    std::this_thread::sleep_for(std::chrono::seconds(100));

    return 0;
}