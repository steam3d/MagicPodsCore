#include <iostream>
#include "DevicesInfoFetcher.h"

using namespace MagicPodsCore;

int main() {
    std::cout << "Hello World!" << std::endl;
    DevicesInfoFetcher devicesInfoFetcher{};

    auto airpodsDevices = devicesInfoFetcher.GetAirpodsInfos();
    for (int i = 0; i < airpodsDevices.size(); ++i) {
        airpodsDevices[i]->Disconnect();
        std::cout << "Name:" << airpodsDevices[i]->GetName() << " Address:" << airpodsDevices[i]->GetAddress() << " Connected:"<< airpodsDevices[i]->GetConnected() << std::endl;
    }

    return 0;
}