#pragma once

#include <string>
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <vector>
#include <cstdint>

namespace MagicPodsCore {

    class StringUtils {
    public:
        static std::string ToUpperCase(const std::string& string) {
            auto newString = string;
            std::transform(newString.begin(), newString.end(), newString.begin(), [](auto symbol) {
                return std::toupper(symbol);
            });
            return newString;
        }

        static std::string ToLowerCase(const std::string& string) {
            auto newString = string;
            std::transform(newString.begin(), newString.end(), newString.begin(), [](auto symbol) {
                return std::tolower(symbol);
            });
            return newString;
        }

        template<typename ... Args>
        static std::string Format(std::string format, const Args&... args) {
            char buffer[512];
            int stringLength = std::sprintf(buffer, format.c_str(), args...);
            return std::string(buffer, stringLength);
        }

        template<typename ... Args>
        static std::string Format(const char* format, const Args&... args) {
            char buffer[512];
            int stringLength = std::sprintf(buffer, format, args...);
            return std::string(buffer, stringLength);
        }

        // 327 -> 0147
        static std::string int16ToHex(int16_t value) {
            std::stringstream ss;
            ss << std::hex << std::setw(4) << std::setfill('0') << value;
            return ss.str();
        }

        static std::vector<char> HexStringToBytes(const std::string& hex) {
	        std::vector<char> bytes;

            for (unsigned int i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                char byte = (char) strtol(byteString.c_str(), NULL, 16);
                bytes.push_back(byte);
            }

            return bytes;
        }

        inline static const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        static std::string BytesToHexString(const unsigned char *data, int len)
        {
            std::string s(len * 2, ' ');
            for (int i = 0; i < len; ++i) {
                s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
                s[2 * i + 1] = hexmap[data[i] & 0x0F];
            }
            return s;
        }

        static std::string BytesToHexString(const std::vector<unsigned char>& data)
        {
            return BytesToHexString(data.data(), data.size());
        };

        static void UuidStringToBytes(const char *uuid_str, uint8_t *uuid_bytes) {
            int byte_index = 0;
            for (int i = 0; uuid_str[i] != '\0'; i++) {
                if (uuid_str[i] == '-') continue;  // Пропускаем дефисы

                // Преобразуем два символа (hex) в байт
                unsigned int byte;
                std::sscanf(&uuid_str[i], "%2x", &byte);
                uuid_bytes[byte_index++] = (uint8_t)byte;

                // Переходим на два символа вперед
                i++;
            }
        }

        // void print_uuid_bytes(const uint8_t *uuid_bytes) {
        //     for (int i = 0; i < 16; i++) {
        //         std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)uuid_bytes[i] << " ";
        //     }
        //     std::cout << std::dec << std::endl;
        // }

        // int main() {
        //     const char *uuid_str = "d8308c4e-9469-4051-8adc-7a2663e415e2";
        //     uint8_t uuid_bytes[16] = {0};

        //     uuid_string_to_bytes(uuid_str, uuid_bytes);

        //     std::cout << "UUID в байтах: ";
        //     print_uuid_bytes(uuid_bytes);

        //     return 0;
        // }
    };
}