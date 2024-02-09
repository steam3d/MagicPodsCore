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

#include "aap.h"

using namespace std; // TODO: delete

// скорее всего внутри будет поток который слушать все входящие сообщения и основной поток будет засылать 
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

        void Process()
        {
            const char* deviceAddr = "FC:1D:43:DE:80:49";
            struct sockaddr_l2 addr = { 0 };
            int sock;
            const char *sample_text = "L2CAP Simple Example Done";

            printf("Start Bluetooth L2CAP client, server addr %s\n", deviceAddr);
            
            /* allocate a socket */
            sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

            /* set the outgoing connection parameters, server's address and port number */
            addr.l2_family = AF_BLUETOOTH;								/* Addressing family, always AF_BLUETOOTH */
            addr.l2_psm = htobs(0x1001);					/* server's port number */
            str2ba(deviceAddr, &addr.l2_bdaddr);		/* server's Bluetooth Address */

            /* connect to server */
            if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("failed to connect");
                exit(1);
            }
            printf("connected...\n");
            
            std::string initialMessage = "00000400000000000000000000000000";
            auto initialMessageBytes = hexStringToBytes(initialMessage);
            ssize_t sendedBytesLength = send(sock, initialMessageBytes.data(), initialMessageBytes.size(), 0);
            std::cout << "i(" << sendedBytesLength << "):" << initialMessage << std::endl;

            char buffer[1024];
            while(true) {
                memset(buffer, 0, sizeof(buffer));
                if (ssize_t receivedBytesLength = recv(sock, buffer, sizeof(buffer), 0); receivedBytesLength >= 0) {
                    std::cout << "o(" << receivedBytesLength << "):" << bytesToHexString(buffer, receivedBytesLength) << std::endl;
                }

                initialMessage.clear();
                std::cout << "enter input:" << std::endl;
                std::getline(std::cin, initialMessage);
                if (!initialMessage.empty()) {
                    auto initialMessageBytes = hexStringToBytes(initialMessage);
                    ssize_t sendedBytesLength = send(sock, initialMessageBytes.data(), initialMessageBytes.size(), 0);
                    std::cout << "i(" << sendedBytesLength << "):" << initialMessage << std::endl;
                }
            }
            
            close(sock);
        }

        bool SetSocketBlockingEnabled(int fd, bool blocking);
    };

}