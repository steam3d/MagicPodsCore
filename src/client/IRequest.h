#include <vector>

namespace MagicPodsCore {

    class IRequest {
    public:
        virtual std::vector<unsigned char> GetRequestData() const = 0;
        virtual ~IRequest() = default;
    };

}