//
// MagicPodsCore
// https://github.com/steam3d/MagicPodsCore
//
// Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: AGPL-3.O
//

#pragma once

#include "IRequest.h"
#include "ClientConnectionType.h"
#include "Event.h"
#include "BlockingQueue.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <array>
#include <memory>
#include <optional>
#include <mutex>
#include <functional>

namespace MagicPodsCore {

    class Client {
    private:
        std::string _address{};
        unsigned short _port{};
        std::string _serviceUuid{};

        ClientConnectionType _connectionType{};

        int _socket{};
        bool _isStarted{false};

        std::mutex _startStopMutex{};

        BlockingQueue<std::vector<unsigned char>> _outcomeMessagesQueue{};

        Event<std::vector<unsigned char>> _onReceivedDataEvent{};

    public:
        void Start(const std::function<void(Client&)>& justAfterStartLogic = {});
        void Stop();

        bool IsStarted() const {
            return _isStarted;
        }

        Event<std::vector<unsigned char>>& GetOnReceivedDataEvent() {
            return _onReceivedDataEvent;
        }

        void SendData(const std::vector<unsigned char>& data);

    private:
        inline bool ConnectToSocketL2CAP();
        inline bool ConnectToSocketRFCOMM();
        inline bool ConnectToSocket();
        inline static std::optional<uint8_t> RetrieveServicePortRFCOMM(uint8_t* uuid, const char* deviceAddress);

    private:
        explicit Client(const std::string& address, unsigned short port, ClientConnectionType connectionType);
        explicit Client(const std::string& address, const std::string& serviceUuid, ClientConnectionType connectionType);

    public:
        static std::unique_ptr<Client> CreateL2CAP(const std::string& address, unsigned short port);
        static std::unique_ptr<Client> CreateRFCOMM(const std::string& address, const std::string& serviceUuid);
        ~Client();
    };
}
