#pragma once

#include <string>
#include <algorithm>
#include <cstdio>

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
    };
}