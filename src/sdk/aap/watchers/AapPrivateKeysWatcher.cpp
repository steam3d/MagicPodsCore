// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapPrivateKeysWatcher.h"
#include "sdk/aap/enums/AapPrivateKeysType.h"
#include "StringUtils.h"

namespace MagicPodsCore
{    
    AapPrivateKeysWatcher::AapPrivateKeysWatcher() : AapWatcher{"AapPrivateKeysWatcher"}
    {
    }

    void AapPrivateKeysWatcher::ProcessResponse(const std::vector<unsigned char> &data)
    {
        if (data.size() < 7)
            return;

        if (data[4] != 0x31)
            return;

        unsigned char count = data[6];
        size_t start = 7;
        AapPrivateKeysArgs keys;

        for (unsigned char i = 0; i < count; i++){
            if (start + 3 >= data.size())
                break;
        
            if (!isValidAapPrivateKeysType(data[start]))
                break;

            AapPrivateKeysType type = static_cast<AapPrivateKeysType>(data[start]);                
            size_t key_len = data[start+2];
            start += 4;

            if (start + key_len > data.size())
                break;
            
            if (type == AapPrivateKeysType::Irk){
                keys.IRK = std::vector<unsigned char>(
                data.begin() + start,
                data.begin() + start + key_len);
            }
            else if (type == AapPrivateKeysType::Enc){
                keys.ENC = std::vector<unsigned char>(
                data.begin() + start,
                data.begin() + start + key_len);
            }
            start += key_len;
        }                                                

        if (keys.IRK.size() != 0 && keys.ENC.size() != 0){
            Logger::Debug("%s: IRK:%s",_tag.c_str(), StringUtils::BytesToHexString(keys.IRK).c_str());
            Logger::Debug("%s: ENC:%s",_tag.c_str(), StringUtils::BytesToHexString(keys.ENC).c_str());            
            Logger::Debug("%s: Successfully got IRC and ENC keys", _tag.c_str());
            _event.FireEvent(keys);
        }
    }
}