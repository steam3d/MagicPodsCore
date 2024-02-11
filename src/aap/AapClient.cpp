#include "AapClient.h"

#include <thread>
#include <sys/ioctl.h>
#include <fcntl.h>

namespace MagicPodsCore {

    AapClient::AapClient(const std::string& address) : _address{address} {
    }

    void AapClient::Start() {
        if (_isStarted)
            return;
        _isStarted = true;

        std::cout << "AapClient started for " << _address << std::endl;
        std::thread thread([this]() {
            struct sockaddr_l2 addr = { 0 };
            const char *sample_text = "L2CAP Simple Example Done";

            printf("Start Bluetooth L2CAP client, server addr %s\n", _address.c_str());
            
            /* allocate a socket */
            _socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

            /* set the outgoing connection parameters, server's address and port number */
            addr.l2_family = AF_BLUETOOTH;								/* Addressing family, always AF_BLUETOOTH */
            addr.l2_psm = htobs(0x1001);					/* server's port number */
            str2ba(_address.c_str(), &addr.l2_bdaddr);		/* server's Bluetooth Address */

            /* connect to server */
            if(connect(_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("failed to connect");
                exit(1);
            }
            printf("connected...\n");
            
            unsigned char buffer[1024];
            std::vector<unsigned char> vectorBuffer(1024); // optimize
            while(_isStarted) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t receivedBytesLength = recv(_socket, buffer, sizeof(buffer), 0);
                if (receivedBytesLength >= 0) {
                    std::cout << "o(" << receivedBytesLength << "):" << bytesToHexString(buffer, receivedBytesLength) << std::endl;
                    vectorBuffer.assign(buffer, buffer + receivedBytesLength);
                    for (auto& watcher : RegisteredWatchers) {
                        watcher->ProcessBytes(vectorBuffer);
                    }
                }
                else if (receivedBytesLength < 0) {
                    break;
                }
            }

            std::cout << "Thread stopped" << std::endl;
        });
        thread.detach();

        std::this_thread::sleep_for(std::chrono::seconds(5));
        SendRequest(AapInit{});
        std::this_thread::sleep_for(std::chrono::seconds(5));
        SendRequest(AapEnableNotifications{NotificationsMode::Unknown1});
        
    }

    void AapClient::Stop() {
        if (!_isStarted)
            return;
        _isStarted = false;

        close(_socket);

        std::cout << "AapClient stopped for " << _address << std::endl;
    }

    void AapClient::SendRequest(const AapRequest& aapRequest) {
        auto requestData = aapRequest.Request();
        ssize_t sendedBytesLength = send(_socket, requestData.data(), requestData.size(), 0);
        std::cout << "i(" << requestData.size() << "):" << bytesToHexString(requestData.data(), requestData.size()) << std::endl;
    }

}