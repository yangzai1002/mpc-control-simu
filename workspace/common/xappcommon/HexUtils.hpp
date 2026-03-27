#ifndef HEX_UTILS_HPP
#define HEX_UTILS_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "SimpleBuffer.hpp"

namespace app_common {
    class HexUtils {
    public:
        HexUtils() = delete;

    public:
        [[maybe_unused]] static std::string to_hex_string(const std::vector<std::uint8_t>& v) {
            std::string ret;
            ret.reserve(v.size() * 2);

            uint8_t tempbyte;
            auto    len = v.size();
            for (decltype(len) i = 0; i < len; i++) {
                tempbyte = v[i];
                tempbyte = (tempbyte >> 4U) & 0x0F;
                if (tempbyte > 9)
                    ret.push_back(tempbyte + '7');
                else
                    ret.push_back(tempbyte + '0');

                tempbyte = v[i];
                tempbyte = tempbyte & 0x0F;
                if (tempbyte > 9)
                    ret.push_back(tempbyte + '7');
                else
                    ret.push_back(tempbyte + '0');
            }

            return ret;
        }

        [[maybe_unused]] static std::string to_hex_string(const std::uint8_t* p, std::size_t len) {
            std::string ret;
            ret.reserve(len * 2);

            uint8_t tempbyte;
            for (decltype(len) i = 0; i < len; i++) {
                tempbyte = p[i];
                tempbyte = (tempbyte >> 4U) & 0x0F;
                if (tempbyte > 9)
                    ret.push_back(tempbyte + '7');
                else
                    ret.push_back(tempbyte + '0');

                tempbyte = p[i];
                tempbyte = tempbyte & 0x0F;
                if (tempbyte > 9)
                    ret.push_back(tempbyte + '7');
                else
                    ret.push_back(tempbyte + '0');
            }

            return ret;
        }

        [[maybe_unused]] static std::string to_hex_string(const SimpleBuffer& b) {
            return to_hex_string(b.data(), b.len());
        }
    };
}; // namespace app_common
#endif
