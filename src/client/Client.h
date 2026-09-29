// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

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
#include <atomic>

namespace MagicPodsCore {

    class Client {
    private:
        static const int CONNECTION_TO_SOCKET_ATTEMPTS_NUMBER = 1;

        std::string _address{};
        unsigned short _port{};
        std::string _serviceUuid{};

        ClientConnectionType _connectionType{};

        int _socket{-1};
        std::atomic<bool> _isStarted{false};
        // Counts the connections made by Start(). The threads of a connection carry its number, so one left over from a connection which was already stopped and replaced can tell it is stale.
        std::atomic<uint64_t> _connectionGeneration{0};

        std::mutex _lifecycleMutex{};
        std::mutex _startStopMutex{};
        // The reader may call StopIfCurrent(), so it is joined without holding _startStopMutex.
        std::thread _writingThread{};
        std::thread _readingThread{};

        BlockingQueue<std::vector<unsigned char>> _outcomeMessagesQueue{};

        Event<std::vector<unsigned char>> _onReceivedDataEvent{};
        Event<std::string> _onConnectionLostEvent{};

    public:
        void Start(const std::function<void(Client&)>& justAfterStartLogic = {});
        void Stop();

        bool IsStarted() const {
            return _isStarted;
        }

        Event<std::vector<unsigned char>>& GetOnReceivedDataEvent() {
            return _onReceivedDataEvent;
        }

        // Fired when the socket dies on its own, never when Stop() closes it.
        Event<std::string>& GetOnConnectionLostEvent() {
            return _onConnectionLostEvent;
        }

        void SendData(const std::vector<unsigned char>& data);

    private:
        inline bool ConnectToSocketL2CAP();
        inline bool ConnectToSocketRFCOMM();
        inline bool ConnectToSocket(int attemptsNumber);
        inline void CloseSocket();
        inline bool StopLocked();
        inline bool StopIfCurrent(uint64_t generation);
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
