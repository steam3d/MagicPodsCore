#include "Aap.h"

#include "Logger.h"

namespace MagicPodsCore {

    AapRequest::AapRequest(std::string tag) : _tag{tag} {
    }

    void AapRequest::PrintAsHex() {
        LOG_DEBUG("%s ", _tag.c_str());
        std::vector<unsigned char> bytes = Request();
        for(int i = 0; i < bytes.size(); i++)
            LOG_DEBUG("%02x", bytes[i]);
        LOG_DEBUG("");
        LOG_DEBUG("");
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
        
        LOG_DEBUG("%s", _tag.c_str());
        
        int batteryCount = static_cast<int>(bytes[6]);

        int startByte = 7;
        std::string readableStr = "";
        std::map<BatteryType, BatteryWatcherData> appBatteryStatus{};
        for (int i = 0; i < batteryCount; i++) {
            BatteryType batteryType = static_cast<BatteryType>(bytes[startByte]);
            unsigned char battery = bytes[startByte + 2];
            ChargingStatus charging = static_cast<ChargingStatus>(bytes[startByte+3]);
            startByte += 5;        
            
            // Sometimes AirPods send strange battery
            battery = battery > 100 ? 100 : battery;
            battery = battery < 0 ? 0 : battery;

            struct BatteryWatcherData batteryData;
            batteryData.Battery = battery;
            batteryData.Status = charging;
            batteryData.Tag = _tag;
            appBatteryStatus[batteryType] = batteryData;
            
            readableStr += DummyConvertBatteryType(batteryType) + " " + std::to_string(battery) + " " + DummyConvertChargingStatus(charging) + "";
        }

        _event.FireEvent(appBatteryStatus);
        LOG_DEBUG("%s", readableStr.c_str());
    }

    std::string AapBatteryWatcher::DummyConvertChargingStatus(ChargingStatus status) {
        switch (status) {
            case ChargingStatus::Undefined:
                return "Undefined";
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
        // min length of the anc packet
        if (bytes.size() < 10) return;

        // packet type must be Settings
        if (bytes[4] != static_cast<unsigned char>(Cmd::Settings)) return;

        // settings type must be Anc
        if (bytes[6] != static_cast<unsigned char>(CmdSettings::Anc)) return;
        
        LOG_DEBUG("%s", _tag.c_str());

        AncMode ancMode = static_cast<AncMode>(bytes[7]);

        // REPLACE WITH ANC STORAGE LOGIC OR EVENT ONANCCHANGED?
        LOG_DEBUG("%s", DummyConvertAncMode(ancMode).c_str());
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