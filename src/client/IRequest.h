// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include <vector>

namespace MagicPodsCore {

    class IRequest {
    public:
        virtual std::vector<unsigned char> GetRequestData() const = 0;
        virtual ~IRequest() = default;
    };

}