#include "Event.h"

#include <string>
#include <optional>
#include <vector>
#include <sdbus-c++/sdbus-c++.h>

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

    struct DBusDeviceInfo {
    private:
        std::string _address{};
        DBusDeviceModalias _modalias{};
        std::vector<std::string> _uuids{};
        std::optional<unsigned int> _clazz{};
        std::string _name{};
        bool _connectionStatus{};

    public:
        explicit DBusDeviceInfo(const std::map<std::string, sdbus::Variant>& interface) 
        {
            _address = interface.at("Address").get<std::string>(),
            _modalias = interface.contains("Modalias") // TODO: modalias
                        ? DBusDeviceModalias{0, 0}
                        : DBusDeviceModalias{std::nullopt, std::nullopt};
            _uuids = std::vector<std::string>{};
            _clazz = interface.contains("Class") 
                        ? std::optional<unsigned int>{ interface.at("Class").get<unsigned int>() } 
                        : std::nullopt;
            _name = interface.at("Name").get<std::string>();
            _connectionStatus = interface.at("Connected").get<bool>();
        }

        DBusDeviceInfo(const DBusDeviceInfo& info) = delete;
        DBusDeviceInfo(DBusDeviceInfo&& info) noexcept = delete;
        DBusDeviceInfo& operator=(const DBusDeviceInfo& info) = delete;
        DBusDeviceInfo& operator=(DBusDeviceInfo&& info) noexcept = delete;

        const std::string GetAddress() const {
            return _address;
        }

        const DBusDeviceModalias& GetModalias() const {
            return _modalias;
        }

        const std::vector<std::string> GetUuids() const {
            return _uuids;
        }

        const std::optional<unsigned int>& GetClass() const {
            return _clazz;
        }

        const std::string& GetName() const {
            return _name;
        }

        bool GetConnectionStatus() const {
            return _connectionStatus;
        }
    };

}