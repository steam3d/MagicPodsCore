// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#pragma once

#include "Event.h"

namespace MagicPodsCore {

    template<typename DataType>
    class ObservableVariable {
    private:
        Event<DataType> _event{};
        DataType _value{};

    public:
        explicit ObservableVariable(DataType initialValue) : _value{initialValue} {
        }

        Event<DataType>& GetEvent() {
            return _event;
        }

        const DataType& GetValue() const {
            return _value;
        }

        void SetValue(const DataType& newValue) {
            _value = std::move(newValue);
            _event.FireEvent(_value);
        }
    };

}