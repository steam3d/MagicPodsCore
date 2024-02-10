#pragma once

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

#include "Aap.h"

namespace MagicPodsCore {

    class AapClient {
    private:
        std::string _address{};

        int _socket{};

        bool _isStarted{false};

    public:
        explicit AapClient(const std::string& address);

        void Start();
        
        void Stop();

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

        std::string bytesToHexString(char *data, int len)
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