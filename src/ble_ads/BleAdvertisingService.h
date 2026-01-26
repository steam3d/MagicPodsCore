// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"
#include "BleAdverisingData.h"

namespace MagicPodsCore
{

    class BleAdvertisingService
    {
    protected:
        Event<BleAdertisingData> _onAdReceivedEvent{};

    public:
        BleAdvertisingService() = default;
        virtual ~BleAdvertisingService() = default;

        Event<BleAdertisingData> &GetOnAdReceivedEvent()
        {
            return _onAdReceivedEvent;
        }

        virtual void StartScan(bool isPassive) = 0; // метод перевода в режим дискавери и установки сканирования в режим le
        virtual void StopScan() = 0; // метод для остановки режима дискавери
        virtual void StartListening() = 0; // метод для начала прослушивания
    };

}
