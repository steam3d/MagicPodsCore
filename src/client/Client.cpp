// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "Client.h"

#include "Logger.h"
#include "StringUtils.h"

#include <thread>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <exception>
#include <cerrno>
#include <cstring>

namespace MagicPodsCore {
    Client::Client(const std::string& address, unsigned short port, ClientConnectionType connectionType)
        : _address{address}, _port{port}, _connectionType{connectionType} {}

    Client::Client(const std::string& address, const std::string& serviceUuid, ClientConnectionType connectionType)
        : _address{address}, _serviceUuid{serviceUuid}, _connectionType{connectionType} {}

    void Client::Start(const std::function<void(Client&)>& justAfterStartLogic) {
        std::lock_guard lifecycleLock{_lifecycleMutex};

        {
            std::lock_guard stateLock{_startStopMutex};
            if (_isStarted)
                return;
        }

        // The reader may finish through StopIfCurrent(), so join it without holding the state mutex.
        if (_readingThread.joinable())
            _readingThread.join();

        std::lock_guard stateLock{_startStopMutex};
        _isStarted = true;

        Logger::Info("%s Start Bluetooth client", _address.c_str());

        /* connect to server */
        if(!ConnectToSocket(CONNECTION_TO_SOCKET_ATTEMPTS_NUMBER)) {
            _isStarted = false;
            Logger::Error("%s Connect to socket is failed.",_address.c_str());
            return;
        }
        Logger::Info("%s connected", _address.c_str());

        // Stop() closes the queue to wake the writer, a restart needs it open again. Whatever was queued while stopped goes out on this connection, as it always did.
        _outcomeMessagesQueue.Open();

        // Both threads work on the descriptor and the generation they were started with instead of the members, which Stop() and a later Start() change under them.
        const uint64_t generation = ++_connectionGeneration;
        const int socket = _socket;

        // Kept joinable so Stop() can wait for it, otherwise a writer left behind by a lost connection would go on taking messages once the socket is replaced.
        _writingThread = std::thread([this, socket]() {
            while (_isStarted) {
                const auto data = _outcomeMessagesQueue.Take();

                if (!data.has_value())
                    break;

                ssize_t sendedBytesLength = send(socket, data.value().data(), data.value().size(), 0);
                Logger::Debug("s:%s",StringUtils::BytesToHexString(data.value().data(), data.value().size()).c_str());
                //std::this_thread::sleep_for(std::chrono::milliseconds{500}); //Return if the user's feedback is bad.
            }

            Logger::Debug("%s Writing thread stopped", _address.c_str());
        });

        _readingThread = std::thread([this, socket, generation]() {
            unsigned char buffer[1024];
            std::vector<unsigned char> vectorBuffer(1024); // optimize
            while(_isStarted && _connectionGeneration == generation) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t receivedBytesLength = recv(socket, buffer, sizeof(buffer), 0);
                if (receivedBytesLength > 0) {
                    Logger::Trace("r:%s", StringUtils::BytesToHexString(buffer, receivedBytesLength).c_str());
                    vectorBuffer.assign(buffer, buffer + receivedBytesLength);

                    _onReceivedDataEvent.FireEvent(vectorBuffer);
                }
                else {
                    Logger::Debug("%s stop listening", _address.c_str());
                    break;
                }
            }

            Logger::Debug("%s Reading thread stopped", _address.c_str());

            // recv() only fails while the client is started when the remote side is gone. The adapter can still report the device as connected, so no disconnect is coming to close the socket and reset the device.
            // Only the reader of the connection still current may tear it down. One left over from a connection Stop() already replaced would otherwise stop the healthy new one and report it lost.
            if (StopIfCurrent(generation)) {
                Logger::Info("%s Connection lost", _address.c_str());
                _onConnectionLostEvent.FireEvent(_address);
            }
        });
        justAfterStartLogic(*this);
    }

    void Client::Stop() {
        std::lock_guard lifecycleLock{_lifecycleMutex};

        {
            std::lock_guard stateLock{_startStopMutex};
            StopLocked();
        }

        if (_readingThread.joinable())
            _readingThread.join();
    }

    bool Client::StopIfCurrent(uint64_t generation) {
        std::lock_guard lockGuard{_startStopMutex};

        if (_connectionGeneration != generation)
            return false;
        return StopLocked();
    }

    // Returns whether this call did the stopping. Expects _startStopMutex to be held.
    bool Client::StopLocked() {
        if (!_isStarted)
            return false;
        _isStarted = false;

        // shutdown() unblocks recv() and send() while the descriptor stays reserved, so neither thread can end up on a descriptor which was reused in the meantime.
        shutdown(_socket, SHUT_RDWR);

        // The writer blocks on the queue, not on the socket. Wake it and wait for it, so the next Start() is the only one writing to the new socket.
        _outcomeMessagesQueue.Close();
        if (_writingThread.joinable())
            _writingThread.join();

        CloseSocket();

        Logger::Info("Stop Bluetooth client, server addr %s", _address.c_str());
        return true;
    }

    void Client::CloseSocket() {
        if (_socket < 0)
            return;

        close(_socket);
        _socket = -1;
    }

    void Client::SendData(const std::vector<unsigned char>& data) {
        _outcomeMessagesQueue.Put(data);
    }

    bool Client::ConnectToSocketL2CAP() {
        struct sockaddr_l2 addr = { 0 };

        /* allocate a socket */
        _socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if (_socket < 0) {
            Logger::Error("%s Failed to create L2CAP socket: %s", _address.c_str(), strerror(errno));
            return false;
        }

        /* set the outgoing connection parameters, server's address and port number */
        addr.l2_family = AF_BLUETOOTH;								/* Addressing family, always AF_BLUETOOTH */
        addr.l2_psm = htobs(0x1001);					/* server's port number */
        str2ba(_address.c_str(), &addr.l2_bdaddr);		/* server's Bluetooth Address */

        /* connect to server */
        if(connect(_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            Logger::Error("%s L2CAP connect failed: %s", _address.c_str(), strerror(errno));
            // A failed attempt must not keep its descriptor, Start() is retried while the adapter still reports the device as connected.
            CloseSocket();
            return false;
        }

        return true;
    }

    bool Client::ConnectToSocketRFCOMM() {
        Logger::Debug("%s is trying to connect to %s", _address.c_str(), _serviceUuid.c_str());

        if (_serviceUuid.empty()){
            Logger::Error("Failed to connect. UUID is empty.");
            return false;
        }

        struct sockaddr_rc addr = { 0 };

        /* retreiving the port, before there is a socket to leak when it fails */
        uint8_t uuid_bytes[16] = {0};
        StringUtils::UuidStringToBytes(_serviceUuid.c_str(), uuid_bytes);
        const auto optionalPort = RetrieveServicePortRFCOMM(uuid_bytes, _address.c_str());
        if (!optionalPort.has_value()) {
            return false;
        }
        _port = optionalPort.value();

        /* allocate a socket */
        _socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (_socket < 0) {
            Logger::Error("%s Failed to create RFCOMM socket: %s", _address.c_str(), strerror(errno));
            return false;
        }

        // set the connection parameters (who to connect to)
        addr.rc_family = AF_BLUETOOTH;
        addr.rc_channel = _port;
        str2ba(_address.c_str(), &addr.rc_bdaddr);

        /* connect to server */
        if(connect(_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            Logger::Error("%s RFCOMM connect failed: %s", _address.c_str(), strerror(errno));
            // A failed attempt must not keep its descriptor, Start() is retried while the adapter still reports the device as connected.
            CloseSocket();
            return false;
        }

        return true;
    }

    bool Client::ConnectToSocket(int attemptsNumber) {
        bool isConnected{false};

        while (true) {
            --attemptsNumber;
            Logger::Info("%s Attempt to connect. Left %d", _address.c_str(), attemptsNumber);
            switch (_connectionType)
            {
            case ClientConnectionType::L2CAP:
                isConnected = ConnectToSocketL2CAP();
                break;
            case ClientConnectionType::RFCOMM:
                isConnected = ConnectToSocketRFCOMM();
                break;
            }
            if (attemptsNumber <= 0 || isConnected || !_isStarted) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1)); // TODO: вероятно delay можно выставить для connect()
        }
        return isConnected;
    }

    std::optional<uint8_t> Client::RetrieveServicePortRFCOMM(uint8_t* uuid, const char* deviceAddress)
    {
        // connect to an SDP server
        uint8_t address[6];
        str2ba(deviceAddress, (bdaddr_t*)&address);

        bdaddr_t tmp = { 0, 0, 0, 0, 0, 0 };

        sdp_session_t* session = sdp_connect(&tmp, (bdaddr_t*)&address, SDP_RETRY_IF_BUSY);
        if (!session) {
            Logger::Error("%s SDP can't connect to sdp server!", deviceAddress);
            return std::nullopt;
        }
        // The session holds a descriptor of its own, closed on every way out of here
        const std::unique_ptr<sdp_session_t, int (*)(sdp_session_t*)> sessionGuard{session, sdp_close};

        uuid_t uuid128;
        sdp_uuid128_create(&uuid128, uuid);

        // create query lists
        int range = 0x0000ffff;
        sdp_list_t* responseList = nullptr;
        sdp_list_t* searchList = sdp_list_append(nullptr, &uuid128);
        sdp_list_t* attrIdList = sdp_list_append(nullptr, &range);

        // The query lists and the records of the answer were never freed. This runs on every connect attempt and the restarts made that a steady leak, so all of it is released on every way out of here.
        using SdpListGuard = std::unique_ptr<sdp_list_t, void (*)(sdp_list_t*)>;
        const SdpListGuard searchListGuard{searchList, [](sdp_list_t* list) { sdp_list_free(list, nullptr); }};
        const SdpListGuard attrIdListGuard{attrIdList, [](sdp_list_t* list) { sdp_list_free(list, nullptr); }};

        // search for records
        int success = sdp_service_search_attr_req(
            session, searchList, SDP_ATTR_REQ_RANGE, attrIdList, &responseList);
        if (success) {
            Logger::Error("%s SDP search failed!", deviceAddress);
            return std::nullopt;
        }

        // Each entry of the answer is a record with an allocation of its own, freed before the list holding them
        const SdpListGuard responseListGuard{responseList, [](sdp_list_t* list) {
            for (sdp_list_t* entry = list; entry; entry = entry->next)
                sdp_record_free((sdp_record_t*)entry->data);
            sdp_list_free(list, nullptr);
        }};

        // check responses
        success = sdp_list_len(responseList);
        if (success <= 0) {
            Logger::Error("%s SDP no responses!", deviceAddress);
            return std::nullopt;
        }

        // process responses
        std::optional<uint8_t> channel{std::nullopt};
        sdp_list_t* responses = responseList;
        while (responses) {
            auto* record = (sdp_record_t*)responses->data;

            sdp_list_t* protoList;
            success = sdp_get_access_protos(record, &protoList);
            if (success) {
                Logger::Error("%s SDP can't access protocols!", deviceAddress);
                return std::nullopt;
            }

            sdp_list_t* protocol = protoList;
            while (protocol) {
                sdp_list_t* pds;
                int protocolCount = 0;
                pds = (sdp_list_t*)protocol->data;

                while (pds) { // loop thru all pds
                    sdp_data_t* d;
                    int dtd;
                    d = (sdp_data_t*)pds->data;
                    while (d) {
                        dtd = d->dtd;
                        switch (dtd) {
                        case SDP_UUID16:
                        case SDP_UUID32:
                        case SDP_UUID128:
                            protocolCount = sdp_uuid_to_proto(&d->val.uuid);
                            break;
                        case SDP_UINT8:
                            if (protocolCount == RFCOMM_UUID) {
                                channel = d->val.uint8; // save channel id
                            }
                            break;
                        default:
                            break;
                        }
                        d = d->next; // to next data unit
                    }
                    pds = pds->next; // to next pds
                }
                sdp_list_free((sdp_list_t*)protocol->data, nullptr);

                protocol = protocol->next; // to next protocol
            }
            sdp_list_free(protoList, nullptr);

            responses = responses->next; // to next response
        }

        return channel;
    }

    std::unique_ptr<Client> Client::CreateL2CAP(const std::string& address, unsigned short port) {
        return std::unique_ptr<Client>(new Client(address, port, ClientConnectionType::L2CAP));
    }

    std::unique_ptr<Client> Client::CreateRFCOMM(const std::string& address, const std::string& serviceUuid) {
        return std::unique_ptr<Client>(new Client(address, serviceUuid, ClientConnectionType::RFCOMM));
    }

    Client::~Client()
    {
        Stop();
        Logger::Debug("Client::~Client()");
    }
}
