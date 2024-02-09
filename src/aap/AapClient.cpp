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
            
            std::string initialMessage = "00000400000000000000000000000000";
            auto initialMessageBytes = hexStringToBytes(initialMessage);
            ssize_t sendedBytesLength = send(_socket, initialMessageBytes.data(), initialMessageBytes.size(), 0);
            std::cout << "i(" << sendedBytesLength << "):" << initialMessage << std::endl;

            char buffer[1024];
            while(true) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t receivedBytesLength = recv(_socket, buffer, sizeof(buffer), 0);
                if (receivedBytesLength >= 0) {
                    std::cout << "o(" << receivedBytesLength << "):" << bytesToHexString(buffer, receivedBytesLength) << std::endl;
                } else {
                    break;
                }

                initialMessage.clear();
            }
            
            close(_socket);

            std::cout << "Thread stopped" << std::endl;
        });
        thread.detach();
    }

    void AapClient::Stop() {
        if (!_isStarted)
            return;
        _isStarted = false;

        int iResult;
        u_long iMode = 0;

        SetSocketBlockingEnabled(_socket, false);
        // int rc = ioctl(_socket, FIONBIO,(char *)&iResult);
        // if (rc < 0) {
        //     perror("ioctl() failed");
        //     // close(_socket);
        // }

        std::cout << "AapClient stopped for " << _address << std::endl;
    }

    bool AapClient::SetSocketBlockingEnabled(int fd, bool blocking) {
        if (fd < 0) return false;

        #ifdef _WIN32
        unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(fd, FIONBIO, &mode) == 0);
        #else
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(fd, F_SETFL, flags) == 0);
        #endif
    }

}