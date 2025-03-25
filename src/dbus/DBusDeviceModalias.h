#include <optional>

namespace MagicPodsCore {

    struct DBusDeviceModalias {
    private:
        std::optional<unsigned short> _productId{};
        std::optional<unsigned short> _vendorId{};

    public:
        explicit DBusDeviceModalias() {}

        explicit DBusDeviceModalias(std::optional<unsigned short> productId, std::optional<unsigned short> vendorId) 
            : _productId{productId}, _vendorId{vendorId} {}

        const std::optional<unsigned short>& GetProductId() const {
            return _productId;
        }

        const std::optional<unsigned short>& GetVectorId() const {
            return _vendorId;
        }
    };

}