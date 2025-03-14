#include <iostream>
#include "client/Client.h"

class CheckRequest : public MagicPodsCore::IRequest {
public:
    std::vector<unsigned char> GetRequestData() const override {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00};
    }
};

class CheckResponseWatcher : public MagicPodsCore::IResponseWatcher {
public:
    void ProcessBytes(const std::vector<unsigned char>& bytes) override {
        // Just stub
    }
};

int main() {
    using namespace MagicPodsCore;

    std::cout << "Hello" << std::endl;

    const auto client = MagicPodsCore::Client::Create("24:29:34:BD:CD:0F", ClientConnectionType::RFCOMM, std::make_shared<CheckResponseWatcher>());
    client->Start();
    
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // client->SendRequest(CheckRequest{});

    std::this_thread::sleep_for(std::chrono::seconds(100));

    return 0;
}