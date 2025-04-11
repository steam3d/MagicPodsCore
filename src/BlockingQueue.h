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
        bool _isDestructed{false};

    public:
        void Put(const TDataType& newValue);
        std::optional<TDataType> Take();
        ~BlockingQueue(){
            {
                std::unique_lock<std::mutex> lock{_putTakeMutex};
                _isDestructed = true;
            }
            _conditionNotEmptyQueue.notify_all();
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
            return !_queue.empty() || _isDestructed;
        });
        if (_isDestructed)
            return std::nullopt;

        const auto value = _queue.front();
        _queue.pop();
        return value;
    }

}