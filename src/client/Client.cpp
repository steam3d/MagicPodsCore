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

namespace MagicPodsCore {
    Client::Client(const std::string& address, unsigned short port, ClientConnectionType connectionType) 
        : _address{address}, _port{port}, _connectionType{connectionType} {}

    Client::Client(const std::string& address, const std::string& serviceUuid, ClientConnectionType connectionType)
        : _address{address}, _serviceUuid{serviceUuid}, _connectionType{connectionType} {}

    void Client::Start(const std::function<void(Client&)>& justAfterStartLogic) {
        std::lock_guard lockGuard{_startStopMutex};

        if (_isStarted)
            return;
        _isStarted = true;

        Logger::Info("Start Bluetooth client, server addr %s", _address.c_str());

        /* connect to server */
        if(!ConnectToSocket(CONNECTION_TO_SOCKET_ATTEMPTS_NUMBER)) {
            _isStarted = false;
            Logger::Info("Connect to socket is failed. Stopping client");
            return;
        }
        Logger::Info("connected...");

        std::thread writingThread([this]() {
            while (_isStarted) {
                const auto data = _outcomeMessagesQueue.Take(); 
                
                if (!data.has_value())
                    break;       
                
                ssize_t sendedBytesLength = send(_socket, data.value().data(), data.value().size(), 0);
                Logger::Debug("s:%s",StringUtils::BytesToHexString(data.value().data(), data.value().size()).c_str());
            }

            Logger::Debug("Writing thread stopped");
        });
        writingThread.detach();

        std::thread readingThread([this]() {           
            unsigned char buffer[1024];
            std::vector<unsigned char> vectorBuffer(1024); // optimize
            while(_isStarted) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t receivedBytesLength = recv(_socket, buffer, sizeof(buffer), 0);
                if (receivedBytesLength > 0) {
                    Logger::Trace("r:%s", StringUtils::BytesToHexString(buffer, receivedBytesLength).c_str());
                    vectorBuffer.assign(buffer, buffer + receivedBytesLength);
                    
                    _onReceivedDataEvent.FireEvent(vectorBuffer);
                }
                else {
                    Logger::Debug("stop listening");
                    break;
                }
            }

            Logger::Debug("Reading thread stopped");
        });
        readingThread.detach();

        justAfterStartLogic(*this);
    }

    void Client::Stop() {
        std::lock_guard lockGuard{_startStopMutex};

        if (!_isStarted)
            return;
        _isStarted = false;

        close(_socket);

        Logger::Info("Stop Bluetooth client, server addr %s", _address.c_str());
    }

    void Client::SendData(const std::vector<unsigned char>& data) {
        _outcomeMessagesQueue.Put(data);
    }

    bool Client::ConnectToSocketL2CAP() {
        struct sockaddr_l2 addr = { 0 };
        
        /* allocate a socket */
        _socket = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

        /* set the outgoing connection parameters, server's address and port number */
        addr.l2_family = AF_BLUETOOTH;								/* Addressing family, always AF_BLUETOOTH */
        addr.l2_psm = htobs(0x1001);					/* server's port number */
        str2ba(_address.c_str(), &addr.l2_bdaddr);		/* server's Bluetooth Address */

        /* connect to server */
        if(connect(_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            return false;
        }

        return true;
    }

    bool Client::ConnectToSocketRFCOMM() {
        struct sockaddr_rc addr = { 0 };
        
        /* allocate a socket */
        _socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

        /* retreiving the port */
        uint8_t uuid_bytes[16] = {0};
        StringUtils::UuidStringToBytes(_serviceUuid.c_str(), uuid_bytes);
        const auto optionalPort = RetrieveServicePortRFCOMM(uuid_bytes, _address.c_str());
        if (!optionalPort.has_value()) {
            return false;
        }
        _port = optionalPort.value();

        // set the connection parameters (who to connect to)
        addr.rc_family = AF_BLUETOOTH;
        addr.rc_channel = _port;
        str2ba(_address.c_str(), &addr.rc_bdaddr);
    
        /* connect to server */
        if(connect(_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            return false;
        }

        return true;
    }

    bool Client::ConnectToSocket(int attemptsNumber) {
        bool isConnected{false};
        while (true) {
            --attemptsNumber;
            Logger::Info("Attempt to connect. Left %d", attemptsNumber);
            switch (_connectionType)
            {
            case ClientConnectionType::L2CAP:
                isConnected = ConnectToSocketL2CAP();
                break;
            case ClientConnectionType::RFCOMM:
                isConnected = ConnectToSocketRFCOMM();
                break;
            }
            if (attemptsNumber == 0 || isConnected || !_isStarted) {
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
            Logger::Info("-- can't connect to sdp server! \n");
            return std::nullopt;
        }

        uuid_t uuid128;
        sdp_uuid128_create(&uuid128, uuid);

        // create query lists
        int range = 0x0000ffff;
        sdp_list_t* responseList;
        sdp_list_t* searchList = sdp_list_append(nullptr, &uuid128);
        sdp_list_t* attrIdList = sdp_list_append(nullptr, &range);

        // search for records
        int success = sdp_service_search_attr_req(
            session, searchList, SDP_ATTR_REQ_RANGE, attrIdList, &responseList);
        if (success) {
            Logger::Debug("-- search failed! \n");
            return std::nullopt;
        }

        // check responses
        success = sdp_list_len(responseList);
        if (success <= 0) {
            Logger::Debug("-- no responses! \n");
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
                Logger::Debug("-- can't access protocols! \n");
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