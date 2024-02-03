#include <iostream>
#include "DevicesInfoFetcher.h"

using namespace MagicPodsCore;

int main() {
    std::cout << "Hello World!" << std::endl;
    DevicesInfoFetcher devicesInfoFetcher{};

    auto airpodsDevices = devicesInfoFetcher.GetAirpodsInfos();
    for (int i = 0; i < airpodsDevices.size(); ++i) {
        airpodsDevices[i]->Disconnect();
        std::cout << airpodsDevices[i]->AsJson() << std::endl;
    }

    return 0;
}