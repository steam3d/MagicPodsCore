#pragma once

#include <functional>
#include <map>

namespace MagicPodsCore {

    template<typename DataType>
    class Event {
    private:
        size_t _idForNewListener{};
        std::map<size_t, std::function<void(size_t, const DataType&)>> _listeners{};

    public:
        // TODO: обработать перемещение

        size_t Subscribe(std::function<void(size_t, const DataType&)>&& listener);
        void Unsubscribe(size_t listenerId);
        void FireEvent(const DataType& dataType);
    };

    template<typename DataType>
    size_t Event<DataType>::Subscribe(std::function<void(size_t, const DataType&)>&& listener) {
        _listeners.emplace(++_idForNewListener, listener);
        return _idForNewListener;
    }

    template<typename DataType>
    void Event<DataType>::Unsubscribe(size_t listenerId) {
        if (_listeners.find(listenerId) != _listeners.end())
            _listeners.erase(listenerId);
    }
    
    template<typename DataType>
    void Event<DataType>::FireEvent(const DataType& dataType) {
        for (auto& [key, value] : _listeners)
            value(key, dataType);
    }

}