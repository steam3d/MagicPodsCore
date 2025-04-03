#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace MagicPodsCore {

    template<typename TDataType>
    class BlockingQueue
    {
    private:
        std::queue<TDataType> _queue{};
        std::mutex _putTakeMutex{};
        std::condition_variable _conditionNotEmptyQueue{};

    public:
        void Put(const TDataType& newValue);
        TDataType Take();
    };

    template<typename TDataType>
    void BlockingQueue<TDataType>::Put(const TDataType& newValue) {
        std::unique_lock<std::mutex> lock{_putTakeMutex};
        _queue.push(newValue);
        _conditionNotEmptyQueue.notify_one();
    }

    template<typename TDataType>
    TDataType BlockingQueue<TDataType>::Take() {
        std::unique_lock<std::mutex> lock{_putTakeMutex};
        _conditionNotEmptyQueue.wait(lock, [this]() {
            return !_queue.empty();
        });
        const auto value = _queue.front();
        _queue.pop();
        return value;
    }

}