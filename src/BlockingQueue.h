// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "Logger.h"
#include <optional>

namespace MagicPodsCore {

    template<typename TDataType>
    class BlockingQueue
    {
    private:
        std::queue<TDataType> _queue{};
        std::mutex _putTakeMutex{};
        std::condition_variable _conditionNotEmptyQueue{};
        bool _isClosed{false};

    public:
        void Put(const TDataType& newValue);
        // Blocks until a value is available or the queue is closed, which returns nothing.
        std::optional<TDataType> Take();
        // Wakes every blocked Take() and makes it return nothing until Open() is called. The queued values are kept.
        void Close();
        void Open();
        ~BlockingQueue(){
            Close();
            Logger::Debug("~BlockingQueue");
        }
    };

    template<typename TDataType>
    void BlockingQueue<TDataType>::Put(const TDataType& newValue) {
        std::unique_lock<std::mutex> lock{_putTakeMutex};
        _queue.push(newValue);
        _conditionNotEmptyQueue.notify_one();
    }

    template<typename TDataType>
    std::optional<TDataType> BlockingQueue<TDataType>::Take() {
        std::unique_lock<std::mutex> lock{_putTakeMutex};
        _conditionNotEmptyQueue.wait(lock, [this]() {
            return !_queue.empty() || _isClosed;
        });
        if (_isClosed)
            return std::nullopt;

        const auto value = _queue.front();
        _queue.pop();
        return value;
    }

    template<typename TDataType>
    void BlockingQueue<TDataType>::Close() {
        {
            std::unique_lock<std::mutex> lock{_putTakeMutex};
            _isClosed = true;
        }
        _conditionNotEmptyQueue.notify_all();
    }

    template<typename TDataType>
    void BlockingQueue<TDataType>::Open() {
        std::unique_lock<std::mutex> lock{_putTakeMutex};
        _isClosed = false;
    }

}