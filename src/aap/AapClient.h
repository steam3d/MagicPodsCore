//
// MagicPodsCore
// https://github.com/steam3d/MagicPodsCore
//
// Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: AGPL-3.O 
//

#pragma once

#include "setters/AapRequest.h"
#include "setters/AapInit.h"
#include "setters/AapEnableNotifications.h"
#include "watchers/AapAncWatcher.h"
#include "watchers/AapBatteryWatcher.h"
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
#include <mutex>
#include "../StringUtils.h"

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

        bool IsStarted() const {
            return _isStarted;
        }

        Event<std::vector<DeviceBatteryData>>& GetBatteryEvent() {
            return BatteryWatcher->GetEvent();
        }

        Event<AncWatcherData>& GetAncEvent() {
            return AncWatcher->GetEvent();
        }

        void SendRequest(const AapRequest& aapRequest);

    private:
        std::vector<char> hexStringToBytes(const std::string& hex) {
	        std::vector<char> bytes;

            for (unsigned int i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                char byte = (char) strtol(byteString.c_str(), NULL, 16);
                bytes.push_back(byte);
            }

            return bytes;
        }
    };

}