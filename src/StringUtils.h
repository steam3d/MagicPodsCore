#pragma once

#include <string>
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <vector>

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

        // 327 -> 0147
        static std::string int16ToHex(int16_t value) {
            std::stringstream ss;
            ss << std::hex << std::setw(4) << std::setfill('0') << value;
            return ss.str();
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
        };

        static std::string BytesToHexString(const std::vector<unsigned char>& data)
        {
            return BytesToHexString(data.data(), data.size());
        };
    };
}