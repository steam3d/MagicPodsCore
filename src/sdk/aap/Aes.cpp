// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "Aes.h"
#include <openssl/evp.h>
#include "StringUtils.h"
#include "Logger.h"

namespace MagicPodsCore{

    bool Aes::VerifyRPA(const std::string &address, const std::string &irk)
    {
        if (irk.size() != 32 || address.size() != 17)
            return false;

        std::vector<uint8_t> key = StringUtils::HexStringToBytes(irk);
        if (key.size() != 16)
            return false;

        std::string mac = address;
        mac.erase(std::remove(mac.begin(), mac.end(), ':'), mac.end());
        std::vector<uint8_t> rpa = StringUtils::HexStringToBytes(mac);
        if (rpa.size() != 6)
            return false;

        std::reverse(rpa.begin(), rpa.end());
        std::vector<uint8_t> hash(rpa.begin(), rpa.begin() + 3);
        std::vector<uint8_t> prand(rpa.begin() + 3, rpa.end());
        
        uint8_t padded[16] = {0};
        std::copy(prand.begin(), prand.end(), padded);        

        uint8_t reversedKey[16];
        std::reverse_copy(key.begin(), key.end(), reversedKey);

        uint8_t reversedData[16];
        std::reverse_copy(padded, padded + 16, reversedData);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return false;

        int out_len1 = 0, out_len2 = 0;
        bool ok = EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, reversedKey, nullptr) == 1;
        ok = ok && EVP_CIPHER_CTX_set_padding(ctx, 0) == 1; //set no padding

        uint8_t out[16];
        ok = ok && EVP_EncryptUpdate(ctx, out, &out_len1, reversedData, 16) == 1;
        ok = ok && EVP_EncryptFinal_ex(ctx, out + out_len1, &out_len2) == 1;
        EVP_CIPHER_CTX_free(ctx);    

        std::reverse(out, out + 16);
        return ok && (out_len1 + out_len2 == 16) && std::equal(hash.begin(), hash.end(), out);
    }
    
    std::vector<unsigned char> Aes::DecryptPayload(const std::string &encPayload, const std::string &enc)
    {
        return DecryptPayload(StringUtils::HexStringToBytes(encPayload), StringUtils::HexStringToBytes(enc));
    }

    std::vector<unsigned char> Aes::DecryptPayload(const std::vector<unsigned char> &encPayload, const std::vector<unsigned char> &enc)
    {
        if (encPayload.size() != 16 || enc.size() != 16) return std::vector<unsigned char>();

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return std::vector<unsigned char>();

        uint8_t iv[16] = {0};
        int out_len1 = 0, out_len2 = 0;
        bool ok = EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, enc.data(), iv) == 1;
        ok = ok && EVP_CIPHER_CTX_set_padding(ctx, 0) == 1;

        uint8_t out[16];
        ok = ok && EVP_DecryptUpdate(ctx, out, &out_len1, encPayload.data(), 16) == 1;
        ok = ok && EVP_DecryptFinal_ex(ctx, out + out_len1, &out_len2) == 1;
        EVP_CIPHER_CTX_free(ctx);

        if (!ok)
            return std::vector<unsigned char>();        

        return std::vector<unsigned char>(out, out + 16);
    }
}