// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AppAnimationCapability.h"
#include "sdk/aap/structs/AapPrivateKeysArgs.h"
#include "sdk/aap/setters/AapPrivateKeys.h"
#include "sdk/aap/AapHelper.h"
#include "sdk/aap/Aes.h"
#include "device/DeviceBattery.h"

namespace MagicPodsCore
{
    nlohmann::json AppAnimationCapability::CreateJsonBody()
    {
        return nlohmann::json::object();
    }

    void AppAnimationCapability::OnReceivedData(const std::vector<unsigned char> &data)
    {
        watcher.ProcessResponse(data);
    }

    AppAnimationCapability::AppAnimationCapability(AapDevice &device) : AapCapability("animation", false, device)
    {
        std::optional<std::string> settingEnc = this->device.LoadSettingString("enc");
        if (settingEnc.has_value())
            enc = settingEnc.value();

        std::optional<std::string> settingIrk = this->device.LoadSettingString("irk");
        if (settingIrk.has_value())
            irk = settingIrk.value();

        std::optional<int64_t> settingColor = this->device.LoadSettingInt("color");
        if (settingColor.has_value())
            hasColor = true;

        leEventId = this->device.GetLeDataReceived().Subscribe([this](size_t id, const BleAdertisingData& adData){

            if (irk.size() == 0 || enc.size() == 0)
            return;

            auto data = adData.GetManufacturerData();

            if (data.size() == 0)
            return;

            for (const auto& [company_id, bytes] : data)
            {
                if (company_id == this->device.GetVendorId() && bytes.size() >= 27 && bytes[0] == 0x07){
                    if (!Aes::VerifyRPA(adData.GetAddress(), irk))
                        continue;

                    std::optional<bleData> toFire{};
                    {
                        std::lock_guard<std::mutex> lock(locker);
                        auto parsedData = AppAnimationCapability::ParseBle(bytes, this->device.GetProductId(), enc);
                        if (parsedData && (!cachedData.has_value() || cachedData.value() != parsedData.value())){
                            //animation state controlled by mac
                            if (parsedData->animation == true && mac == ""){
                                mac = adData.GetAddress();
                                cachedData = parsedData.value();
                                
                                if (!hasColor){
                                    this->device.SaveSettingInt("color", cachedData.value().color);
                                    hasColor = true;
                                }
                                
                                toFire = parsedData.value();
                                Logger::Critical("%s\n%s", adData.GetAddress().c_str(), cachedData.value().ToString().c_str());
                            }
                            else if (parsedData->animation == false && mac != ""){
                                mac = "";
                                cachedData = parsedData.value();
                                toFire = parsedData.value();
                                Logger::Critical("%s\n%s", adData.GetAddress().c_str(), parsedData.value().ToString().c_str());
                                Logger::Critical("Waiting animation true");
                            }
                            else if (parsedData->animation == true && mac == adData.GetAddress()){
                                cachedData = parsedData.value();
                                toFire = parsedData.value();
                                Logger::Critical("Updating animation");
                            }
                        }
                    }
                    if (toFire.has_value())
                        FireAnimation(toFire.value());
                }
            }
        });

        connectionEventId = this->device.GetConnectedPropertyChangedEvent().Subscribe([this](size_t id, bool isConnected)
        {
            std::optional<bleData> toFire{};
            {
                std::lock_guard<std::mutex> lock(locker);
                if (cachedData.has_value() && cachedData.value().animation == true && mac != ""){
                    toFire = cachedData.value();
                    Logger::Critical("Connection animation");
                }
            }
            if (toFire.has_value())
                FireAnimation(toFire.value());
        });

        watcherEventId = watcher.GetEvent().Subscribe([this](size_t id, AapPrivateKeysArgs mode)
                                                    {
                                                        // Save to headphones settings
                                                        this->irk = StringUtils::BytesToHexString(mode.IRK);
                                                        this->device.SaveSettingString("irk", this->irk);
                                                        this->enc = StringUtils::BytesToHexString(mode.ENC);
                                                        this->device.SaveSettingString("enc", this->enc);
                                                    });
    }

    AppAnimationCapability::~AppAnimationCapability()
    {
        watcher.GetEvent().Unsubscribe(watcherEventId);
        this->device.GetConnectedPropertyChangedEvent().Unsubscribe(connectionEventId);
        this->device.GetLeDataReceived().Unsubscribe(leEventId);
    }

    void AppAnimationCapability::FireAnimation(const bleData &bleData)
    {
            auto responseJson = nlohmann::json::object();
            auto animationJson = nlohmann::json::object();
            animationJson["address"] = device.GetAddress();
            animationJson["name"] = device.GetName();
            animationJson["model"] = device.GetProductId();
            animationJson["vendor"] = device.GetVendorId();
            animationJson["connected"] = device.GetConnected();
            animationJson["color"] = bleData.color;
            animationJson["show"] = bleData.animation;

            DeviceBattery battery(false);
            battery.UpdateBattery(bleData.batteryData);
            animationJson["battery"] = battery.CreateJsonBody();

            responseJson["animation"] = animationJson;
            device.FireAnimation(responseJson);
    }

    bool AppAnimationCapability::IsAnimationSupport(unsigned short productId)
    {
        switch (static_cast<AapModelIds>(productId))
        {
        case AapModelIds::airpods1:
        case AapModelIds::airpods2:
        case AapModelIds::airpods3:
        case AapModelIds::airpodspro:
        case AapModelIds::airpodspro2:
        case AapModelIds::airpodspro3:
        case AapModelIds::airpodsprousbc:
        case AapModelIds::airpodsmax:
        case AapModelIds::powerbeatspro:
        case AapModelIds::beatsfitpro:
        case AapModelIds::beatsstudiobuds:
        case AapModelIds::beatsstudiobudsplus:
        case AapModelIds::airpodsmax2024:
        case AapModelIds::airpods4anc:
        case AapModelIds::airpods4:
        case AapModelIds::powerbeatspro2:
        case AapModelIds::powerbeatsfit:
        case AapModelIds::airpodsmax2:
            return true;
        default:
            return false;
        }
    }

    uint8_t AppAnimationCapability::ClampBattery(uint8_t value)
    {
        if (value < 0)
            value = 0;
        else if (value > 10)
            value = 0; // disconnected
        return value * 10;
    }

    uint8_t AppAnimationCapability::ClampNativeBattery(uint8_t value)
    {
        if (value < 0)
            value = 0;
        else if (value > 100)
            value = 0; // disconnected
        return value;
    }
    std::optional<bleData> AppAnimationCapability::ParseBle(const std::vector<uint8_t> &buffer, unsigned short model, const std::string &enc)
    {

        // We skips fake AirPods to avoid getting incorrect data
        if (buffer.size() < 27)
            return std::nullopt;

        if (buffer[0] != 0x07 || buffer[1] != 0x19)
            return std::nullopt;

        // Data is corrupted
        if (buffer[1] + 2 != buffer.size())
            return std::nullopt;

        uint16_t leModel = (static_cast<uint16_t>(buffer[4]) << 8) | buffer[3];


        if (leModel != model || !IsAnimationSupport(model))
            return std::nullopt;

        bleData data;
        data.model = static_cast<AapModelIds>(leModel);
        data.color = buffer[9];

        uint8_t capCount = buffer[8];
        uint8_t utp = buffer[5];
        bool _bothAirPodsInCase = utp & 0b0000'0100;
        bool _twoAirPodActive = utp & 0b0000'0001;
        bool isLeft = utp & 0b0010'0000 ? true : false;

        if (data.model == AapModelIds::airpodsmax || data.model == AapModelIds::airpodsmax2024)
        {
            data.animation = (capCount >> 4) == 8 ? true : false;
        }
        else
        {
            auto lowNibble = capCount & 0x0f;
            data.animation = _bothAirPodsInCase && _twoAirPodActive && lowNibble <= 8 ? true : false;
        }

        std::vector<uint8_t> encPayload{};
        if (enc.size() == 32)
            encPayload = Aes::DecryptPayload(std::vector<unsigned char>(buffer.begin() + 11, buffer.end()), StringUtils::HexStringToBytes(enc));

        // Private battery
        if (encPayload.size() >= 4 && (encPayload[0] == 0x04 || encPayload[0] == 0x14))
        {
            //Logger::Trace("%s", StringUtils::BytesToHexString(encPayload).c_str());
            bool single = (encPayload[1] == 0 || encPayload[2] == 0);
            if (single)
            {
                if (encPayload[1] != 0)
                {
                    data.batteryData.emplace_back(
                        DeviceBatteryType::Single,
                        encPayload[1] == 0xff ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                        ClampNativeBattery(encPayload[1] & 0b0111'1111),
                        encPayload[1] == 0xff ? false : (encPayload[1] & 0b1000'0000) != 0);
                }
                else if (encPayload[2] != 0)
                {
                    data.batteryData.emplace_back(
                        DeviceBatteryType::Single,
                        encPayload[2] == 0xff ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                        ClampNativeBattery(encPayload[2] & 0b0111'1111),
                        encPayload[2] == 0xff ? false : (encPayload[2] & 0b1000'0000) != 0);
                }
                else
                {
                    // Something wrong
                }
            }
            else
            { // 00 -unavailable ff - disconnected
                data.batteryData.emplace_back(
                    DeviceBatteryType::Left,
                    (isLeft ? encPayload[1] : encPayload[2]) == 0xff ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                    ClampNativeBattery(isLeft ? encPayload[1] & 0b0111'1111 : encPayload[2] & 0b0111'1111),
                    (isLeft ? encPayload[1] : encPayload[2]) == 0xff ? false : isLeft ? (encPayload[1] & 0b1000'0000) != 0
                                                                                      : (encPayload[2] & 0b1000'0000) != 0);

                data.batteryData.emplace_back(
                    DeviceBatteryType::Right,
                    (isLeft ? encPayload[2] : encPayload[1]) == 0xff ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                    ClampNativeBattery(isLeft ? encPayload[2] & 0b0111'1111 : encPayload[1] & 0b0111'1111),
                    (isLeft ? encPayload[2] : encPayload[1]) == 0xff ? false : isLeft ? (encPayload[2] & 0b1000'0000) != 0
                                                                                      : (encPayload[1] & 0b1000'0000) != 0);

                if (encPayload[3] != 0)
                {
                    data.batteryData.emplace_back(
                        DeviceBatteryType::Case,
                        encPayload[3] == 0xff ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                        ClampNativeBattery(encPayload[3] & 0b0111'1111),
                        encPayload[3] == 0xff ? false : (encPayload[3] & 0b1000'0000) != 0);
                }
            }
        }
        // Public battery
        else
        {
            bool isSingle = ((buffer[6] & 0b1111'0000) == 0 || (buffer[6] & 0b0000'1111) == 0);
            if (isSingle)
            {
                if ((buffer[6] & 0b11110000) != 0)
                {
                    data.batteryData.emplace_back(
                        DeviceBatteryType::Single,
                        (buffer[6] >> 4) > 0x0a ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                        ClampBattery(buffer[6] >> 4),
                        (buffer[7] & 0b0010'0000) != 0);
                }
                else if ((buffer[6] & 0b0000'1111) != 0)
                {
                    data.batteryData.emplace_back(
                        DeviceBatteryType::Single,
                        (buffer[6] & 0b0000'1111) > 0x0a ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                        ClampBattery(buffer[6] & 0b0000'1111),
                        (buffer[7] & 0b0001'0000) != 0);
                }
                else
                {
                    // Something wrong
                }
            }
            else
            {
                data.batteryData.emplace_back(
                    DeviceBatteryType::Left,
                    (isLeft ? buffer[6] & 0x0f : buffer[6] >> 4) > 0x0a ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                    ClampBattery(isLeft ? buffer[6] & 0x0f : buffer[6] >> 4),
                    isLeft ? (buffer[7] & 0b0001'0000) != 0 : (buffer[7] & 0b0010'0000) != 0);

                data.batteryData.emplace_back(
                    DeviceBatteryType::Right,
                    (isLeft ? buffer[6] >> 4 : buffer[6] & 0x0f) > 0x0a ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                    ClampBattery(isLeft ? buffer[6] >> 4 : buffer[6] & 0x0f),
                    isLeft ? (buffer[7] & 0b0010'0000) != 0 : (buffer[7] & 0b0001'0000) != 0);

                if ((buffer[7] & 0x0f) != 0)
                {
                    data.batteryData.emplace_back(
                        DeviceBatteryType::Case,
                        (buffer[7] & 0x0f) > 0x0a ? DeviceBatteryStatus::Disconnected : DeviceBatteryStatus::Connected,
                        ClampBattery(buffer[7] & 0x0f),
                        (buffer[7] & 0b0100'0000) != 0);
                }
            }
        }
        return data;
    }
}
