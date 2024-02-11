#include "Aap.h"

namespace MagicPodsCore {

    AapRequest::AapRequest(std::string tag) : _tag{tag} {
    }

    void AapRequest::PrintAsHex() {
        printf("%s \n", _tag.c_str());
        std::vector<unsigned char> bytes = Request();
        for(int i = 0; i < bytes.size(); i++)
            printf("%02x", bytes[i]);
        printf("\n");
        printf("\n");
    }

    AapInit::AapInit() : AapRequest{"AapInit"} {
    }

    std::vector<unsigned char> AapInit::Request() const {
        return std::vector<unsigned char>{0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }

    AapEnableNotifications::AapEnableNotifications(NotificationsMode mode) : AapRequest{"AapEnableNotifications"}, _mode{mode} {
    }

    std::vector<unsigned char> AapEnableNotifications::Request() const {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(Cmd::Notifications), 0x00, 0xff, 0xff, static_cast<unsigned char>(_mode), 0xff};
    }

    AapSetAnc::AapSetAnc(AncMode mode) : AapRequest{"AapSetAnc"}, _mode{mode} {
    }

    std::vector<unsigned char> AapSetAnc::Request() const {
        return std::vector<unsigned char>{0x04, 0x00, 0x04, 0x00, static_cast<unsigned char>(Cmd::Settings), 0x00, static_cast<unsigned char>(CmdSettings::Anc), static_cast<unsigned char>(_mode), 0x00, 0x00, 0x00};
    }

    AapWatcher::AapWatcher(std::string tag) : _tag{tag} {
    }

    AapBatteryWatcher::AapBatteryWatcher() : AapWatcher{"AapBatteryWatcher"} {
    }

    // !MUST BE TESTED ON ALL AIRPOD MODES! 
    void AapBatteryWatcher::ProcessBytes(const std::vector<unsigned char>& bytes) {
        // min length of the battery packet
        if (bytes.size() < 11) return;

        // packet type must be battery
        if (bytes[4] != static_cast<unsigned char>(Cmd::Battery)) return;
        
        std::cout << _tag << std::endl;
        
        int batteryCount = static_cast<int>(bytes[6]);

        int startByte = 7;
        std::string readableStr = "";
        for (int i = 0; i < batteryCount; i++) {
            BatteryType batteryType = static_cast<BatteryType>(bytes[startByte]);
            unsigned char battery = bytes[startByte + 2];
            ChargingStatus charging = static_cast<ChargingStatus>(bytes[startByte+3]);
            startByte += 5;
            
            // REPLACE WITH BATTERY STORAGE LOGIC OR EVENT ONBATTERYCHANGED?
            readableStr += DummyConvertBatteryType(batteryType) + " " + std::to_string(battery) + " " + DummyConvertChargingStatus(charging) + "\n";

            _event.FireEvent(BatteryWatcherData{_tag, batteryType, charging, battery});
        }

        std::cout << readableStr << std::endl;
    }

    std::string AapBatteryWatcher::DummyConvertChargingStatus(ChargingStatus status) {
        switch (status) {
            case ChargingStatus::Charging:
                return "Charging";
            case ChargingStatus::NotCharging:
                return "NotCharging";
            case ChargingStatus::Disconnected:
                return "Disconnected";
            default:
                return "Unknown";
        }
    }

    std::string AapBatteryWatcher::DummyConvertBatteryType(BatteryType status) {
        switch (status) {
            case BatteryType::Single:
                return "Single";
            case BatteryType::Left:
                return "Left  ";
            case BatteryType::Right:
                return "Right ";
            case BatteryType::Case:
                return "Case  "; 
            default:
                return "Unknown";
        }
    }

    AapAncWatcher::AapAncWatcher() : AapWatcher{"AapAncWatcher"} {
    }

    void AapAncWatcher::ProcessBytes(const std::vector<unsigned char>& bytes) {
        // min length of the battery packet
        if (bytes.size() < 10) return;

        // packet type must be battery
        if (bytes[4] != static_cast<unsigned char>(Cmd::Settings)) return;
        
        std::cout << _tag << std::endl;

        AncMode ancMode = static_cast<AncMode>(bytes[7]);

        // REPLACE WITH ANC STORAGE LOGIC OR EVENT ONANCCHANGED?
        std::cout << DummyConvertAncMode(ancMode) << std::endl;
        _event.FireEvent(AncWatcherData{_tag, ancMode});
    }

    std::string AapAncWatcher::DummyConvertAncMode(AncMode mode) {
        switch (mode) {
            case AncMode::On:
                return "On";
            case AncMode::Off:
                return "Off";
            case AncMode::Tra:
                return "Tra"; 
            default:
                return "Unknown";
        }
    }   

}