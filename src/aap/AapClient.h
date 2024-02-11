#pragma once

#include "Aap.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <array>
#include <memory>

namespace MagicPodsCore {

    class AapClient {
    private:
        std::unique_ptr<AapBatteryWatcher> BatteryWatcher {std::make_unique<AapBatteryWatcher>()};
        std::unique_ptr<AapAncWatcher> AncWatcher {std::make_unique<AapAncWatcher>()};

        std::string _address{};
        int _socket{};
        bool _isStarted{false};
        
        std::mutex _startStopMutex{};

    public:
        explicit AapClient(const std::string& address);

        void Start();
        void Stop();

        Event<BatteryWatcherData>& GetBatteryEvent() {
            return BatteryWatcher->GetEvent();
        }

        Event<AncWatcherData>& GetAncEvent() {
            return AncWatcher->GetEvent();
        }

    private:
        void SendRequest(const AapRequest& aapRequest);

        std::vector<char> hexStringToBytes(const std::string& hex) {
	        std::vector<char> bytes;

            for (unsigned int i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                char byte = (char) strtol(byteString.c_str(), NULL, 16);
                bytes.push_back(byte);
            }

            return bytes;
        }

        inline static const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        std::string bytesToHexString(unsigned char *data, int len)
        {
            std::string s(len * 2, ' ');
            for (int i = 0; i < len; ++i) {
                s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
                s[2 * i + 1] = hexmap[data[i] & 0x0F];
            }
            return s;
        }
    };

}