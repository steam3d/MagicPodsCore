#include <iostream>
#include "client/Client.h"
#include "Logger.h"
#include "aap/setters/AapInit.h"
#include "aap/setters/AapEnableNotifications.h"

class CheckRequest : public MagicPodsCore::IRequest {
public:
    std::vector<unsigned char> GetRequestData() const override {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00};
    }
};

int main() {
    using namespace MagicPodsCore;

    std::cout << "Hello" << std::endl;

    //const auto client = MagicPodsCore::Client::CreateRFCOMM("24:29:34:BD:CD:0F", "df21fe2c-2515-4fdb-8886-f12c4d67927c");
    const auto client = MagicPodsCore::Client::CreateL2CAP("0A:E1:61:02:A1:F8", 0x1001);
    client->GetOnReceivedDataEvent().Subscribe([](size_t id, const std::vector<unsigned char>& data) {
        LOG_DEBUG("Data received %d", data.size());
    });
    client->Start();
    

    std::this_thread::sleep_for(std::chrono::seconds(1));
    client->SendData(AapInit{}.Request());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client->SendData(AapEnableNotifications{AapNotificationsMode::Unknown1}.Request());

    std::this_thread::sleep_for(std::chrono::seconds(100));

    return 0;
}